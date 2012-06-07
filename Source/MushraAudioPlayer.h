#ifndef MUSHRAAUDIOPLAYER_H
#define MUSHRAAUDIOPLAYER_H

#include "../JuceLibraryCode/JuceHeader.h"
#include "TestLauncher.h"

class TestLauncher; // forward declaration

/* ---------------------------------------------------------- */
class MushraAudioPlayer: public AudioIODeviceCallback
{ 
public:

	MushraAudioPlayer(TestLauncher*	tLauncher);
	~MushraAudioPlayer();

	/* Implementation of the AudioIODeviceCallback method. */
	virtual void audioDeviceIOCallback (const float** inputChannelData,
								int totalNumInputChannels,
								float** outputChannelData,
								int totalNumOutputChannels,
								int numSamples);

	/* Implementation of the AudioIODeviceCallback method. */
	virtual void audioDeviceAboutToStart (AudioIODevice* device);

	/* Implementation of the AudioIODeviceCallback method. */
	virtual void audioDeviceStopped();

	void	switchStimulus(int	newStimulusNumber);
	bool	isRunning() {return playerRunning;};

	int		getCurrentPosition(){ return currentAudioPosition;};
	int		getTotalLength()  {return totalAudioLength;};

	void	setPlayLoop(bool shouldPlayLoop) {playInLoop = shouldPlayLoop;};
	bool	setPosition(int newSamplePosition);
	bool	setFragmentStartPosition(int startPosition);
	bool	setFragmentEndPosition(int endPosition);

	void	pause() { playerPaused = true; };
	void	resume() { playerPaused = false; };




//==============================================================================
	juce_UseDebuggingNewOperator

protected:

	int					totalAudioLength;
	int					currentAudioPosition;
	int					startPosition;
	int					endPosition;
	bool				playInLoop;
	bool				playbackFinished;

	int					currentStimulus;
	int					nextStimulus;

	volatile bool		playerRunning;
	volatile bool		playerPaused;
	TestLauncher*		testLauncher;

	int					stimCount;
	int					channelCount;

	float*				curBuffer;

	AudioSampleBuffer*	crossSampleBuffer;
	
	int					bufferInBytes;
	int					samplesToCopy;

private:
	//==============================================================================
    // (prevent copy constructor and operator= being generated.)
	const MushraAudioPlayer& operator= (const MushraAudioPlayer&);
};


#endif
