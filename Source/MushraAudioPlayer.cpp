/*  
	MUSHRATest - Multichannel implementation of MUSHRA listening test
    Copyright (C) 2011  Roman Kosobrodov

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "MushraAudioPlayer.h"
#include "TestLauncher.h"

//==============================================================================
MushraAudioPlayer::MushraAudioPlayer (TestLauncher*	tLauncher):
					testLauncher(tLauncher),
					totalAudioLength(0),
					currentAudioPosition(0),
					currentStimulus(-1),
					nextStimulus(-1),
					playerRunning(false),
					playerPaused(false),
					playbackFinished(false),
					channelCount(-1),
					startPosition(0),
					endPosition(0),
					bufferInBytes(0),
					samplesToCopy(0),
					playInLoop(false)
{
	if (testLauncher != NULL)
		stimCount = testLauncher->getStimCount();
	else
		stimCount = -1;
}

//==============================================================================
MushraAudioPlayer::~MushraAudioPlayer ()
{
}

//==============================================================================
void MushraAudioPlayer::switchStimulus(int	newStimulusNumber)
{
	playerPaused = false;

	if (playbackFinished)
	{
		setPosition(startPosition);
		playbackFinished = false;
	}	


	if ((newStimulusNumber >= 0) && (newStimulusNumber < stimCount) )
	{
		nextStimulus = newStimulusNumber;
	}
}

//==============================================================================
bool MushraAudioPlayer::setPosition(int newSamplePosition)
{
	if (newSamplePosition == currentAudioPosition)
		return true;

	currentAudioPosition = newSamplePosition;
	playbackFinished = false;
	return true;
}


//==============================================================================
bool MushraAudioPlayer::setFragmentStartPosition(int newStartPosition)
{
	if (newStartPosition == startPosition)
		return false;

	startPosition = newStartPosition;
	return true;
}

//==============================================================================
bool MushraAudioPlayer::setFragmentEndPosition(int newEndPosition)
{
	if (newEndPosition == endPosition)
		return false;

	endPosition = newEndPosition;
	return true;
}

//==============================================================================
void MushraAudioPlayer::audioDeviceAboutToStart (AudioIODevice* device)
{
	int samples = testLauncher->audioDeviceSettings->getBufferSize();
	
	channelCount = testLauncher->getChannelCount();
	totalAudioLength = int(testLauncher->getLengthInSamples());
	crossSampleBuffer = new AudioSampleBuffer(channelCount, samples);

	currentStimulus = -1;
	nextStimulus = -1;

	playerRunning = true;
	playerPaused = true;
	playbackFinished = false;

	testLauncher->dbgOut( L"--- MushraAudioPlayer::audioDeviceAboutToStart ---" );
	testLauncher->dbgOut( L"currentAudioPosition = " + String(currentAudioPosition) );	
	testLauncher->dbgOut( L"startPosition = " + String(startPosition) );
	testLauncher->dbgOut( L"endPosition = " + String(endPosition) );
	testLauncher->dbgOut( L"playInLoop = " + String(playInLoop) );
	testLauncher->dbgOut( L"channelCount = " + String(channelCount) );
	testLauncher->dbgOut( L"playerRunning = " + String(playerRunning) );
	testLauncher->dbgOut( L"playerPaused = " + String(playerPaused) );
	testLauncher->dbgOut( L"device buffer = " + String(device->getCurrentBufferSizeSamples()) );

}


//==============================================================================
void MushraAudioPlayer::audioDeviceStopped ()
{
	delete crossSampleBuffer;
	
	playInLoop = false;
	playerRunning = false;

	testLauncher->dbgOut( L"--- MushraAudioPlayer::audioDeviceStopped ---" );

}


//==============================================================================
void MushraAudioPlayer::audioDeviceIOCallback ( const float** /*inputChannelData*/,
												int /*totalNumInputChannels*/,
												float** outputChannelData,
												int totalNumOutputChannels,
												int numSamples)
{
	/* clear all channels */
	bufferInBytes = numSamples * sizeof(float);
	for (int ch=0; ch < totalNumOutputChannels; ch++)
	{
		zeromem(outputChannelData[ch], bufferInBytes);
	}

	if ( playerPaused || playbackFinished )
		return;

	if (currentAudioPosition >= endPosition)
	{
		if (playInLoop)
			currentAudioPosition = startPosition;
		else
		{
			playbackFinished = true;
			return;
		}
	}
	
	/* get the number of samples available */
	if ( totalAudioLength - currentAudioPosition < numSamples )
	{
		samplesToCopy = totalAudioLength - currentAudioPosition;
	}
	else
	{	
		samplesToCopy = numSamples;
	}
	bufferInBytes = samplesToCopy * sizeof(float);

	/* play selected stimulus */
	if (currentStimulus != -1)
	{
		/* continue playing the stimulus */
		for (int ch=0; ch < channelCount; ch++)
		{
			curBuffer =  testLauncher->getAudioData(currentStimulus, ch, currentAudioPosition);
			if ( curBuffer != NULL )
			{
				memcpy(	outputChannelData[ch], curBuffer, bufferInBytes);
			}
		}

		/* cross-fade if stimuli were switched */
		if (currentStimulus != nextStimulus)
		{
			/* do cross-fade */
			crossSampleBuffer->setDataToReferTo(outputChannelData, channelCount, samplesToCopy);
			for (int ch=0; ch<channelCount; ch++)
			{
				curBuffer =  testLauncher->getAudioData(nextStimulus, ch, currentAudioPosition);
				if ( curBuffer != NULL )
				{					
					crossSampleBuffer->applyGainRamp(ch, 0, samplesToCopy, 1.0f, 0.0f); // fade-out
					crossSampleBuffer->addFromWithRamp(ch, 0, curBuffer, samplesToCopy, 0.0f, 1.0f); // add with fade-in
				}
			}
			currentStimulus = nextStimulus;
		}
	}
	else
	{
		/* start playing the first selected stimulus */
		if (nextStimulus != -1)
		{
			/* fade in */
			crossSampleBuffer->setDataToReferTo(outputChannelData, channelCount, samplesToCopy);
			for (int ch=0; ch<channelCount; ch++)
			{
				curBuffer =  testLauncher->getAudioData(nextStimulus, ch, currentAudioPosition);
				if ( curBuffer != NULL )
				{
					crossSampleBuffer->copyFromWithRamp(ch, 0, curBuffer, samplesToCopy, 0.0f, 1.0f);
				}
			}
			currentStimulus = nextStimulus;
		}
	}

	/* go to the next audio buffer */
	currentAudioPosition += samplesToCopy;

}

