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

#include "TestLauncher.h"

//=========================================================================================================
void Trial::randomiseOrder(int64 seed)
{
	Random		rand(seed);
	Array<int>	tempArray;
	int			ind;
	int			N = filesOrder.size();
	
	while ( N-- > 0)
	{
		ind = rand.nextInt(N+1);
		tempArray.add(filesOrder[ind]);
		filesOrder.remove(ind);
	}
	tempArray.add(filesOrder[0]);
	
	filesOrder.clear();	
	filesOrder.addArray(tempArray);
}

//=========================================================================================================
bool Trial::isValid()
{
	// One of the stimuli in each test MUST have a grade of 100
	bool found = false;

	for (int i=0; i < responses.size(); i++)
	{
		if (responses[i] == 100)
			found = true;
	}

	return found;
}

//=========================================================================================================
void Trial::saveResults(File& f)
{
	f.appendText( L"#------------------------------------\n" , false, false  );
	f.appendText( L"# Test: " + testName + "\n", false, false   );
	f.appendText( L"# stimulus\t response\n" , false, false  );
	
	String tmp;	
	String sep(File::separatorString); 
	int lastIndex;
	for (int i=0; i<soundFiles.size(); i++)
	{
		tmp = soundFiles[filesOrder[i]];
		lastIndex = tmp.lastIndexOf( sep );
		tmp = tmp.substring(lastIndex+1);
		f.appendText( tmp + L"\t" + String(responses[i]) + L"\n", false, false );		
	}
}

// ============================================================================================
TestLauncher::TestLauncher(AudioDeviceSettings*	deviceSettings): 
												audioDeviceSettings(deviceSettings),												
												stimCount(deviceSettings->getStimuliCount()),
												trialsCount(-1),
												currentIndex(-1),
												mushraAudioPlayer(NULL),
												resultsDirectory(String::empty),
												ThreadWithProgressWindow( L"Loading stimuli into memory ", true, false)
{
	audioDeviceManager = new AudioDeviceManager();
	audioFormatManager.registerFormat(new WavAudioFormat(),		 true);
	
	fileLogger = FileLogger::createDefaultAppLogger( L"MUSHRATest", L"mushra.log.txt", String::empty, 0 );	

}


// ============================================================================================
TestLauncher::~TestLauncher()
{
	if (mushraAudioPlayer != NULL)
	{
		if (mushraAudioPlayer->isRunning())
		{
			stop();
		}
	}
	
	delete fileLogger;
	delete audioDeviceManager;
	delete mushraAudioPlayer;

	audioFormatManager.clearFormats();

	releaseAudioData();
	
}

// ============================================================================================
void TestLauncher::dbgOut(const String	msg)
{		
	fileLogger->logMessage(msg);
}

// ============================================================================================
bool TestLauncher::init(String stimuliDir)
{	
	stimuliDirectory = stimuliDir;
	
	bool res = readTrialSettings();
	
	if (!res)
	{
		return false;
	}

	debugTrialSettings();

	/* read audio stimuli */
	runThread();

	if (lastError.isEmpty())
	{	
		mushraAudioPlayer = new MushraAudioPlayer(this);
	}
	
	return res;
}

// ============================================================================================
bool TestLauncher::initDevice(XmlElement* devSettings)
{	
	/* Initialise the device */
	lastError = audioDeviceManager->initialise (	MAXNUMBEROFDEVICECHANNELS,	/* number of input channels */
													MAXNUMBEROFDEVICECHANNELS,	/* number of output channels */
													devSettings,
													false				/* select default device on failure */ );


	if (lastError.isNotEmpty())
	{
		return false;
	}

	
	/* get the settings from the device */
	AudioIODevice* device = audioDeviceManager->getCurrentAudioDevice();
	if (device == NULL)
	{
		lastError = String( L"AudioDeviceManager::getCurrentAudioDevice() returned NULL");
		return false;
	}
	
	return true;
}


// ============================================================================================
Trial*	TestLauncher::getCurrentTest()
{
	if ( (currentIndex >= 0)&&(currentIndex < tests.size() ))
		return tests[currentIndex];
	else
		return NULL;
}

// ============================================================================================
Trial*	TestLauncher::getTest(int ind)
{
	if ( (ind >= 0)&&(ind < tests.size() ))
		return tests[ind];
	else
		return NULL;
}

// ============================================================================================
String	TestLauncher::getCurrentTestName()
{
	if ( (currentIndex >= 0)&&(currentIndex < tests.size() ))
		return tests[currentIndex]->testName;
	else
		return String::empty;
}

// ============================================================================================
bool	TestLauncher::goForward()
{
	if (currentIndex < tests.size() - 1)
	{
		currentIndex++;
		/* update audio data */

		dbgOut( L"\t Starting test " + String(currentIndex) + L"\t" + tests[currentIndex]->testName );

		releaseAudioData();
		runThread();
		return true;
	}
	else
	{
		return false;
	}
}

// ============================================================================================
bool	TestLauncher::goBack()
{
	if (currentIndex > 0)
	{
		currentIndex--;
		return true;
	}
	else
	{
		return false;
	}
}

// ============================================================================================
bool	TestLauncher::goToTrial(int trialIndex)
{
	if ((trialIndex >= 0)&&(trialIndex < tests.size()))
	{
		currentIndex = trialIndex;
		/* update audio data */
		releaseAudioData();
		runThread();
		return true;
	}
	else
	{
		return false;
	}
}

// ============================================================================================
void	TestLauncher::stop()
{
	if (!mushraAudioPlayer->isRunning())
		return;

	/* terminate the playback */
	audioDeviceManager->removeAudioCallback(mushraAudioPlayer);
	/* wait for mushraAudioPlayer to finish */
	while (mushraAudioPlayer->isRunning())
	{
	}
}


// ============================================================================================
bool	TestLauncher::start(XmlElement*	devSettings)
{
	if (initDevice(devSettings))
	{
		audioDeviceManager->addAudioCallback(mushraAudioPlayer);
		return true;
	}
	else
	{
		return false;
	}
}

// ============================================================================================
bool TestLauncher::readTrialSettings()
{
	if (stimuliDirectory.isEmpty())
		return false;
	
	
	File*	stimDir = new File(stimuliDirectory);
	String	tmpString;

	if ( stimDir->exists() && stimDir->isDirectory() )
	{
		/* get the child directories */
		Array<File> childDir;

		tests.clear();

		trialsCount = stimDir->findChildFiles(childDir, File::findDirectories, true);
		stimCount = audioDeviceSettings->getStimuliCount();

		delete stimDir;
		
		if (trialsCount == 0)
		{
			lastError = L"Unable to find subdirectories in " + stimuliDirectory;
			return false;
		}
		else
		{
			/* fill tests array */
			for (int i=0; i < trialsCount; i++)
			{
				tests.add(new Trial);
				/* get stimuli files */
				Array<File> stimuliFiles;
				int stimFound = childDir[i].findChildFiles(stimuliFiles, File::findFiles, false, L"*.wav" );

				if ( stimFound != stimCount )
				{
					lastError = L"The number of stimuli found in " + childDir[i].getFullPathName() 
								+ L" is " + String(stimFound) + L" while " + String(stimCount)+ L" stimuli is expected." ;
					return false;
				}

				/* fill the fields of Trials */
				tests[i]->testName = childDir[i].getFileName();
				for (int j=0; j<stimFound; j++)
				{
					tests[i]->soundFiles.add( stimuliFiles[j].getFullPathName() );
					tests[i]->filesOrder.add(j);
					tests[i]->responses.add(0);
					/* try and find the reference */
					tmpString = stimuliFiles[j].getFileName().toUpperCase();
					if ( tmpString.contains(L"REFERENCE"))
					{
						tests[i]->refIndex = j;							
					}
				}
				
				if ( tests[i]->refIndex == -1 )
				{
					lastError = L"No reference stimulus is found in " + childDir[i].getFullPathName(); 
					return false;				
				}

				if ( audioDeviceSettings->getRandomised() )
				{
					tests[i]->randomiseOrder((int64) tests[i]);
				}

			}
		}
	}
	else
	{
		lastError = L"Unable to find stimuli directory " + stimuliDirectory;
		return false;

	}
	
	if (trialsCount > 0)
	{
		currentIndex = 0;
	}

	return true;
}

// ============================================================================================
bool TestLauncher::saveResults(String	resultFile)
{
	File fs( resultFile );
	if (fs == File::nonexistent)
	{
		lastError = L"Unable to create output file " + resultFile;
		dbgOut(lastError);
		return false;	
	}

	if (fs.existsAsFile())
	{
		dbgOut( L"Deleting existing file:\t" + resultFile );
		fs.deleteFile();
	}
	else
	{
		Result resCreate = fs.create(); 
		if ( resCreate.failed() )
		{
			dbgOut(L"Unable to create directory:\t" + fs.getFullPathName() + ".Error message: " + resCreate.getErrorMessage() );
			
			File docFolder( File::getSpecialLocation( File::userDocumentsDirectory ) );
			File mFolder ( docFolder.getChildFile(L"MUSHRATest") );
			
			if (!mFolder.isDirectory())
			{
				Result res = mFolder.createDirectory();
				if (res.failed())
				{
					lastError = res.getErrorMessage();
					dbgOut( lastError );
					return false;
				}
			}

			fs = mFolder.getChildFile( fs.getFileName() );
			
			resultsDirectory = fs.getParentDirectory().getFullPathName();
		}
	}
	
	/* save comments */
	fs.appendText( L"# date:\t" + Time::getCurrentTime().toString(true,true,false,true) + L"\n", false, false  );
	fs.appendText( L"# subjectID:\t" + subjectID + L"\n", false, false   );
	fs.appendText( L"#\n", false, false  );

	for (int i=0; i<trialsCount; i++)
	{
		tests[i]->saveResults(fs);
	}

	return true;
}

// ============================================================================================
bool TestLauncher::saveResults()
{
	String tmpString( stimuliDirectory ) ;
	Time	now =  Time::getCurrentTime();
	
	tmpString += now.formatted(L"%Y-%m-%d_%H%M%S_") + subjectID + ".txt";

	return saveResults(tmpString);
}

// ============================================================================================
void TestLauncher::switchStimulus(int stimIndex)
{
	mushraAudioPlayer->switchStimulus(stimIndex);
}

// ============================================================================================
bool TestLauncher::isRunning()
{
	return mushraAudioPlayer->isRunning();
}

// ============================================================================================
double TestLauncher::getCurrentPosition()
{
	return mushraAudioPlayer->getCurrentPosition();
}

// ============================================================================================
void TestLauncher::setPlayLoop(bool shouldPlayLoop)
{
	mushraAudioPlayer->setPlayLoop(shouldPlayLoop);
}


// ============================================================================================
double TestLauncher::getTotalLength()
{
	return mushraAudioPlayer->getTotalLength() ;
}


// ============================================================================================
bool TestLauncher::setPosition(float newSamplePosition)
{
	unsigned int newpos ( static_cast<unsigned int> (floor(newSamplePosition*samplesCount) ) );
	if (newpos < samplesCount)
	{	
		return mushraAudioPlayer->setPosition(newpos);
	}
	else
	{
		return false;
	}
}

// ============================================================================================
bool TestLauncher::setFragmentStartPosition(float startPosition)
{
	int newpos = int(floor(startPosition*samplesCount));
	return mushraAudioPlayer->setFragmentStartPosition(newpos);
}

// ============================================================================================
bool TestLauncher::setFragmentEndPosition(float endPosition)
{
	int newpos = int(floor(endPosition*samplesCount));
	return mushraAudioPlayer->setFragmentEndPosition(newpos);
}

// ============================================================================================
void	TestLauncher::pause()
{
	mushraAudioPlayer->pause();
}

// ============================================================================================
void	TestLauncher::resume()
{
	mushraAudioPlayer->resume();
}

//==============================================================================
void	TestLauncher::debugBuffer(float** buf, int bufferCount, int singleBufferSize, String bufferVariableName )
{
	if (buf == NULL)
		return;
	
	String s = String::empty;
	for (int ch = 0; ch<bufferCount; ch++)
	{
		dbgOut(bufferVariableName + L"[" + String(ch) +  L"]:");
		s = String::empty;
		for (int ind =0; ind < singleBufferSize; ind++)
			s = s+ String::formatted( L"%5.4f\t", *(buf[ch] + ind) );
		dbgOut(s);
	}
}

//==============================================================================
void	TestLauncher::debugBuffer(float* buf, int bufferSize, String bufferVariableName )
{
	if (buf == NULL)
		return;

	String s = String::empty;

	s = String::empty;
	for (int ind =0; ind < bufferSize; ind++)
		s = s+ String::formatted( L"%5.4f\t", buf[ind] );
	dbgOut(  bufferVariableName + L" = [" + s + L"];" );
}

//==============================================================================
void	TestLauncher::debugBuffer(int** buf, int bufferCount, int singleBufferSize, String bufferVariableName )
{
	if (buf == NULL)
		return;

	String s = String::empty;
	for (int ch = 0; ch<bufferCount; ch++)
	{
		dbgOut(bufferVariableName + L"[" + String(ch) +  L"]:");
		s = String::empty;
		for (int ind =0; ind < singleBufferSize; ind++)
			s = s+ String::formatted( L"%d\t", *(buf[ch] + ind) );
		dbgOut(s);
	}
}

//==============================================================================
void	TestLauncher::debugBuffer(int* buf, int bufferSize, String bufferVariableName )
{
	if (buf == NULL)
		return;

	String s = String::empty;

	s = String::empty;
	for (int ind =0; ind < bufferSize; ind++)
		s = s+ String::formatted( L"d\t", buf[ind] );
	dbgOut(  bufferVariableName + L" = [" + s + L"];" );
}


//==============================================================================	
void	TestLauncher::debugBuffer(bool* buf, int bufferSize, String bufferVariableName )
{
	if (buf == NULL)
		return;

	String s = String::empty;

	s = String::empty;
	for (int ind =0; ind < bufferSize; ind++)
		s = s+ String( buf[ind] );
	dbgOut(  bufferVariableName + L" = [" + s + L"];" );
}


// ============================================================================================
void	TestLauncher::releaseAudioData()
{
	for (int i = 0; i < audioStimData.size() ; i++)
	{
		for (int ch =0; ch < chanCount; ch++)
		{
			delete [] audioStimData[i][ch];
		}
	}
	audioStimData.clear();
}

// ============================================================================================
void	TestLauncher::run()
{
	bool res = false;
	int bsize = audioDeviceSettings->getBufferSize();
	unsigned int deviceChan = audioDeviceSettings->getOutputChannels();
	int nbuff;
	int remsamples;
	double progressMax;

	releaseAudioData();

	for (int i = 0; i<stimCount; i++)
	{
                if (threadShouldExit())
                    break;
		
			/* Create input stream for the audio file */
			File af(getCurrentTest()->soundFiles[i]);
			wavReader = audioFormatManager.createReaderFor( af );

			dbgOut( L"Loading file " + af.getFullPathName() ); 	

			if (wavReader == NULL)
			{
				lastError = L"Unable to create reader for  " + getCurrentTest()->soundFiles[i];
				return ;
	
			}

			/* get length and number of channels */
			if (wavReader->numChannels > deviceChan)
			{
				lastError = L"The number of audio channels in stimuli files exceeds available device outputs.";
				return ;
			}

			chanCount = wavReader->numChannels;
			samplesCount = static_cast<unsigned int> ( wavReader->lengthInSamples );
			
			/* adjust to the integer number of audio buffers */
			nbuff = div(samplesCount,bsize).quot;
			remsamples = div(samplesCount,bsize).rem;
			
			if (remsamples > 0)
			{
				samplesCount = bsize*(nbuff+1);
				progressMax = stimCount*(nbuff+1);
			}
			else
			{
				progressMax = stimCount*nbuff;
			}
				

			/* create audio data buffers */
			currentData = new int* [chanCount];
			if (currentData == NULL)
			{
				lastError = L"Unable to allocate " + String(2*chanCount*sizeof(float*) + L"b of memory." );
				return ;
			}

			/* create an array of pointers to data buffers */
			for (int ch = 0; ch < chanCount; ch++)
			{
				currentData[ch] = new int[samplesCount];
				if (currentData[ch] == NULL)
				{
					lastError = L"Unable to allocate " + String(samplesCount*sizeof(float*) + L"b of memory." );
					/* clean up (data is massive) */
					for (int k = 0; k<ch; k++)
					{
						delete [] currentData[k];
					}
					delete [] currentData;
					releaseAudioData();
					return ;
				}
				/* clear memory */
				//zeromem(currentData[ch], sizeof(int)*samplesCount);
			}
			/* add array to audioStimData */
			audioStimData.add(currentData);

			/* read file in memory */			
			for (int k=0; k<nbuff; k++)
			{
				res = wavReader->readSamples( currentData, chanCount, bsize*k, bsize*k, bsize);
				if (!res)
				{
					releaseAudioData();
					lastError = L"Unable to read data from file " + getCurrentTest()->soundFiles[i];
					return ;
				}

				/* update progress bar */
				setProgress((i*nbuff + k)/progressMax);
			}
			/* read the last buffer and zero pad */
			if (remsamples > 0)
			{
				/* zero-padding is done by AudioFormatReader */
				res = wavReader->readSamples( currentData, chanCount, bsize*nbuff, bsize*nbuff, bsize);
				if (!res)
				{
					releaseAudioData();
					lastError = L"Unable to read data from file " + getCurrentTest()->soundFiles[i];
					return ;
				}
			}

			/* clean up */
			delete wavReader;
	}

	/*for (int k = 0; k<chanCount; k++)
	{		
		debugBuffer(getAudioData(0,k,48000), bsize*5, L"audio") + String(k));
	}*/

	setProgress(1.0);
	wait(500);
}

// ============================================================================================
float*	TestLauncher::getAudioData(int stimulus, int channel, int startPosition)
{
	if (audioStimData.size() < stimCount)
		return NULL;

	int** tmp2Ptr;
	int* tmpPtr;

	if ((startPosition >=0 ) && ( startPosition < int(samplesCount) ))
	{
		tmp2Ptr = audioStimData[stimulus];
		tmpPtr = tmp2Ptr[channel] + startPosition;
		return reinterpret_cast <float*> (tmpPtr);
	}
	else
		return NULL;
}

// ============================================================================================
void	TestLauncher::debugTrialSettings()
{
	String tmpStr = String::empty;

	for (int i=0; i<tests.size(); i++)
	{
		dbgOut( L"---  " + tests[i]->testName + L"  ---" );
		for (int k = 0; k < tests[i]->soundFiles.size(); k++)
		{
			tmpStr = String(tests[i]->filesOrder[k]) + L"\t " +  tests[i]->soundFiles[k];
			if (k == tests[i]->refIndex)
			{
				tmpStr += L"\t-reference";
			}
			dbgOut(tmpStr);
		}
	}
}
