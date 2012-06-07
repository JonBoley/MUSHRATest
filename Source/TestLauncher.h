#ifndef TESTLAUNCHER_H
#define TESTLAUNCHER_H

#include "../JuceLibraryCode/JuceHeader.h"
#include "MushraAudioPlayer.h"

#define	MAXNUMBEROFDEVICECHANNELS	64

class MushraAudioPlayer; // forward declaration

/* ---------------------------------------------------------- */
class Trial
{
public:
	Trial(): refIndex(-1)	
	{
	};
	
	  ~Trial() {};

	void	randomiseOrder(int64 seed);
	bool	isValid();
	void	saveResults(File& f);

	StringArray	soundFiles;
	Array<int>	filesOrder;
	Array<int>	responses;
	int			refIndex;
	String		testName;

};


/* ---------------------------------------------------------- */
// error codes for audio device settings
#define	NOERROR				0
#define FILENOTOPENED		1
#define FILEERROR			2
#define DEVICERATEERROR		3
#define BUFFERSIZEERROR		4
#define OUTCHANNELSERROR	5
#define STIMULICOUNTERROR	6
#define	DIRECTORYERROR		7
#define	SUBJECTSNAMEERROR	8
#define STIMULIBUFFERERROR	6


/* ---------------------------------------------------------- */
class AudioDeviceSettings
{
public:
	AudioDeviceSettings():	sampleRate(-1.0),
							bufferSize(-1),
							bitDepth(-1),
							outputChannels(-1),
							outputChannelsStr(String::empty),
							xmlSettings(String::empty),
							settingsApplied(false),
							deviceSettings(NULL),
							stimuliBufferSize(4),
							stimCount(0),
							randomiseStimuli(true),
							errorStatus(NOERROR)
	{
		errorStr.add(String::empty);
		errorStr.add(L"AudioDeviceSettings failed:\tUnable to open xml settings file");
		errorStr.add(L"AudioDeviceSettings failed:\tUnable to read from xml settings file");
		errorStr.add(L"AudioDeviceSettings failed:\tWrong or missing parameter audioDeviceRate");
		errorStr.add(L"AudioDeviceSettings failed:\tWrong or missing parameter audioDeviceBufferSize");
		errorStr.add(L"AudioDeviceSettings failed:\tWrong or missing parameter audioDeviceOutChans");
		errorStr.add(L"AudioDeviceSettings failed:\tWrong or missing parameter stimuliCount");
		errorStr.add(L"AudioDeviceSettings failed:\tWrong or missing parameter stimuliDirectory");
		errorStr.add(L"AudioDeviceSettings failed:\tWrong or missing parameter subjectsFile");
		errorStr.add(L"AudioDeviceSettings failed:\tWrong or missing parameter stimuliBufferSize");
	};

	~AudioDeviceSettings() 
	{
		errorStr.clear();
		delete deviceSettings;
	};
	
	int		errorStatus;
	String	getErrorString() 
		{ if ( errorStatus < errorStr.size() )  
			return errorStr[errorStatus] + L"\n\t" + xmlSettings;
		  else
			return L"AudioDeviceSettings failed: Unknown error code";
		};

	//bool	init();
	
	//int		readSettings();
	//bool	getSettings(double &samplRate,int &bufSize, int &bDepth, int &outChannels);

	String	getXmlSettingsFile() { return xmlSettings; }
	//bool	setXmlSettingsFile(String xmlSettingsFile);

	String	getOutputChannelsStr() {return outputChannelsStr;}
	int		getOutputChannels() {return outputChannels;}

	int		getBufferSize() { return bufferSize; };
	double	getSampleRate() { return sampleRate; };
	int		getStimuliBufferSize() {return stimuliBufferSize; }

	int		getRandomised() { return randomiseStimuli; };
	int		getStimuliCount() { return stimCount; };
	
	String		getStimuliDirectory(int ind) 
	{
		if ( (ind >= 0)&&(ind < stimDirectory.size()) )
		{
			return stimDirectory[ind]; 
		}
		else
		{	
			return String::empty;
		}
	};
	
	StringArray*	getSubjects() {return &subjectNames; };

	//XmlElement*	getXmlDeviceSettings();

	//=========================================================================================================
bool init()
{ 
	if (settingsApplied)
		return true;
	else
		return (readSettings() == NOERROR);
}

//=========================================================================================================
int readSettings()
{
	errorStatus = NOERROR;

	if (!settingsApplied)
	{

		File fs( xmlSettings );
		if ( !fs.existsAsFile() )
		{
			errorStatus = FILENOTOPENED;
			return errorStatus;
		}
		
		XmlDocument myDocument (fs);
		
		if (deviceSettings != NULL) // clear previous settings
			delete deviceSettings;
		
		deviceSettings = myDocument.getDocumentElement();

		if (deviceSettings == NULL)
		{
			errorStatus = FILEERROR;
			return errorStatus;
		}
		else
		{
			
			sampleRate	= deviceSettings->getIntAttribute( L"audioDeviceRate", -1 );
			if (sampleRate == -1)
			{
				errorStatus = DEVICERATEERROR;
			}
			
			bufferSize	= deviceSettings->getIntAttribute( L"audioDeviceBufferSize", -1 );
			if (bufferSize == -1)
			{
				errorStatus = BUFFERSIZEERROR;
			}
			bitDepth	= 32;
			
			outputChannelsStr = deviceSettings->getStringAttribute( L"audioDeviceOutChans", String::empty );
			outputChannels = outputChannelsStr.length();
			if ( outputChannelsStr.isEmpty() )
			{
				errorStatus = OUTCHANNELSERROR;
			}

			stimuliBufferSize = deviceSettings->getIntAttribute( L"stimuliBufferSize", -1 );
			if (stimuliBufferSize < 1)
			{
				errorStatus = STIMULIBUFFERERROR;
			}


			stimCount = deviceSettings->getIntAttribute( L"stimuliCount", -1 );
			if ((stimCount == -1) || (stimCount < 3) || (stimCount > 12))
			{
				errorStatus = STIMULICOUNTERROR;
			}
			randomiseStimuli = deviceSettings->getBoolAttribute( L"randomiseStimuli", true );

			/* read all subjects' names and corresponding stimuli directories */
			subjectNames.clear();
			stimDirectory.clear();
			String tmpString(String::empty);

			XmlElement* child = deviceSettings->getFirstChildElement();
			while (child != 0)
			{	
				/* name */
				tmpString = child->getStringAttribute( L"name", String::empty  );
				if (tmpString.isEmpty())
				{
					errorStatus = SUBJECTSNAMEERROR;
				}
				else
				{
					subjectNames.add(tmpString);
				}

				/* stimuli directory */
				tmpString = child->getStringAttribute( L"stimuliDirectory", String::empty );
				if ( tmpString.isEmpty() )
				{
					errorStatus = DIRECTORYERROR;
				}
				else /* check if directory exists */
				{
					File* fs = new File(tmpString);
					if ( (!fs->exists()) || (!fs->isDirectory()) )
					{
						errorStatus = DIRECTORYERROR;					
					}
					else
					{
						tmpString = fs->getFullPathName() + File::separator;
						stimDirectory.add(tmpString);
					}
					delete fs;
				}				
				/* go to the next element */
				child = child->getNextElement();
			}

		}
	}
	
	if ( errorStatus != NOERROR )
	{
		settingsApplied = false;
		delete deviceSettings;
		deviceSettings = NULL;
	}
	else
	{
		settingsApplied = true;
	}

	return errorStatus;
}



// ============================================================================================
bool setXmlSettingsFile(String xmlSettingsFile)
{ 
	xmlSettings = xmlSettingsFile;
	settingsApplied = false;
	return (readSettings() == NOERROR);
};

// ============================================================================================
bool getSettings(double &samplRate,int &bufSize, int &bDepth, int &outChannels)
{
	int res = NOERROR;

	/* apply settings from the xmlSettings file when required */
	if (!settingsApplied)
	{
		res = readSettings();
	}

	if ( res == NOERROR )
	{
		samplRate = sampleRate;
		bufSize = bufferSize;
		bDepth = bitDepth;
		outChannels = outputChannels;
	}
	else
	{
		samplRate = -1.0;
		bufSize = -1;
		bDepth = -1;
		outChannels = -1;	
	}


	return ( res == NOERROR);
}

// ============================================================================================
XmlElement* getXmlDeviceSettings()
{
	/* apply settings from the xmlSettings file when required */
	if (!settingsApplied)
		readSettings();

	return deviceSettings;
}


	juce_UseDebuggingNewOperator

private:
	bool		settingsApplied;
	double		sampleRate;
	int			bufferSize;
	int			bitDepth;
	int			outputChannels;
	String		outputChannelsStr;
	int			stimuliBufferSize;

	String		xmlSettings;
	XmlElement*	deviceSettings;
	StringArray errorStr;

	int			stimCount;
	bool		randomiseStimuli;

	StringArray		subjectNames;
	StringArray		stimDirectory;
};





/* ---------------------------------------------------------- */
class TestLauncher: public ThreadWithProgressWindow
{ 
public:
	TestLauncher(AudioDeviceSettings*	deviceSettings);
	~TestLauncher();

	String				lastError;
	AudioDeviceSettings*	audioDeviceSettings;


	Trial*	getCurrentTest();
	Trial*	getTest(int ind);
	String	getCurrentTestName();
	int		getCurrentTestIndex() {return currentIndex;};
	bool	goForward();
	bool	goBack();
	bool	goToTrial(int trialIndex);
	int		getTrialsCount() {return trialsCount;};
	int		getStimCount() {return stimCount;};
	void	setSubjectID(String newID) {subjectID = newID; };
	void	setStimuliDirectory(String newDir) {stimuliDirectory = newDir;};

	/* Timer method */
	//void 	timerCallback();
	
	/* initialise device settings and test parameters */
	bool	init(String xmlSettingsFile);
	
	/* playback controls */
	bool	start(XmlElement*	devSettings);
	void	stop();

	void	pause();
	void	resume();

	void	switchStimulus(int stimIndex);
	bool	isRunning();
	double	getCurrentPosition();
	double	getTotalLength();
	bool	setPosition(float newSamplePosition);
	bool	setFragmentStartPosition(float startPosition);
	bool	setFragmentEndPosition(float endPosition);
	void	setPlayLoop(bool shouldPlayLoop);

	/* save results of MUSHRA test */
	bool	saveResults();
	bool	saveResults(String	resultFile);
	bool	saveTempResults(){ return saveResults( stimuliDirectory + L"temp_" + subjectID  + L".txt"); };
	String  getResultsDir() { return resultsDirectory; };

	int		getChannelCount() { return chanCount;};
	int64	getLengthInSamples() { return samplesCount;};

	void	dbgOut(const String msg);
	void	debugBuffer(float** buf, int bufferCount, int singleBufferSize, String bufferVariableName );
	void	debugBuffer(float* buf, int bufferSize, String bufferVariableName );
	void	debugBuffer(int** buf, int bufferCount, int singleBufferSize, String bufferVariableName );
	void	debugBuffer(int* buf, int bufferSize, String bufferVariableName );
	void	debugBuffer(bool* buf, int bufferSize, String bufferVariableName );

	float*	getAudioData(int stimulus, int channel, int startPosition);

	void	run();

	juce_UseDebuggingNewOperator

private:
	bool	initDevice(XmlElement* devSettings);
	void	debugTrialSettings();
	
	AudioDeviceManager*		audioDeviceManager;
	MushraAudioPlayer*		mushraAudioPlayer;

	String					subjectID;
	String					stimuliDirectory;
	OwnedArray<Trial>		tests;
	int						stimCount;
	int						trialsCount;
	int						currentIndex;

	FileLogger*				fileLogger;

	OwnedArray<int*>		audioStimData;
	
	int**					currentData;
	AudioFormatReader*		wavReader;
	AudioFormatManager		audioFormatManager;

	/* private methods */
	int						chanCount;
	unsigned int			samplesCount;
	bool					readTrialSettings();
	void					releaseAudioData();

	String					resultsDirectory;
};

#endif
