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

#include "MushraConfigComponent.h"

//==============================================================================
MushraConfigComponent::MushraConfigComponent ()
    : tabbedComponent (0),
      finishButton (0),
	  deviceSelector(0),
	  subjectTableListBox(0),
	  subjectsTabComponent(0)
{
    addAndMakeVisible (tabbedComponent = new TabbedComponent (TabbedButtonBar::TabsAtTop));
    tabbedComponent->setTabBarDepth (30);
	Colour tabBackground( Colours::aliceblue );

    addAndMakeVisible (finishButton = new TextButton (L"finish button"));
    finishButton->setButtonText (L"Finish");
    finishButton->addListener (this);
	
	deviceManager.initialise (2, 2, 0, true, String::empty, 0);
	
	
	/* Create the app data directory if required */
	File tmpDirFile( File::getSpecialLocation( File::userApplicationDataDirectory ).getFullPathName() + File::separatorString + L"MUSHRATest" );
	if ( !tmpDirFile.isDirectory() )
	{
		if ( !tmpDirFile.createDirectory() )
		{
			AlertWindow::showMessageBox( AlertWindow::WarningIcon, L"Error", L"Unable to create directory:\t" + tmpDirFile.getFullPathName(), L"OK", this);
			JUCEApplication::getInstance()->quit();
		}
	}

	File xmlFile = tmpDirFile.getChildFile(L"settings.xml");
	
	settingsFileName =	xmlFile.getFullPathName();

	if ( xmlFile.existsAsFile() )
	{	
		bool res = audioDeviceSettings.setXmlSettingsFile(settingsFileName);

		if (!res)
		{	
			AlertWindow::showMessageBox( AlertWindow::WarningIcon, L"Warning", audioDeviceSettings.getErrorString(), L"OK", this);
		}
	
	}
	else
	{
		String errorMsg( L"Unable to find settings file:\t");
		errorMsg += settingsFileName + "\n\nThe file will be created.";
		AlertWindow::showMessageBox( AlertWindow::InfoIcon, L"Warning", errorMsg, L"OK", this);
	}

	
	/* Initialise the device configuration tab */
	deviceManager.initialise (	MAXNUMBEROFDEVICECHANNELS,	/* number of input channels */
								MAXNUMBEROFDEVICECHANNELS,	/* number of output channels */
								audioDeviceSettings.getXmlDeviceSettings(),
								false				/* select default device on failure */ );

	deviceSelector = new AudioDeviceSelectorComponent (deviceManager, 0, 2, 0, 2, false, false, false, false);
    tabbedComponent->addTab (L"Audio Device Setup",tabBackground, deviceSelector, true);
    
	/* initialise subjects and stimuli configuration tab */
	subjectTableListBox = new SubjectTableListBox(&table, settingsFileName );
	table.setModel(subjectTableListBox);

	table.setColour (ListBox::outlineColourId, Colours::grey);
    table.setOutlineThickness (1);
	
    table.setMultipleSelectionEnabled (false);
	table.getHeader().setStretchToFitActive (true);

	subjectsTabComponent = new SubjectsTabComponent(&table);
	subjectsTabComponent->setBoundsInset(BorderSize<int> (5));

	XmlElement* tmpCols;
	int ww = table.getWidth();
	for (int i = 0; i < subjectTableListBox->getNumColumns(); ++i)
	{
		tmpCols =  subjectTableListBox->getColumns()->getChildElement(i);
		table.getHeader().addColumn (	tmpCols->getStringAttribute ("name"),
                                        tmpCols->getIntAttribute ("columnId"),
										(int) (tmpCols->getDoubleAttribute ("width")*ww),
                                        50, -1,
                                        TableHeaderComponent::defaultFlags);
	}

	tabbedComponent->addTab (L"Test Configuration", tabBackground, subjectsTabComponent, true);
	tabbedComponent->setCurrentTabIndex (0);
    
    setSize (800, 580);
}

MushraConfigComponent::~MushraConfigComponent()
{
	delete subjectTableListBox;

    deleteAndZero (tabbedComponent);
    deleteAndZero (finishButton);

}

//==============================================================================
void MushraConfigComponent::paint (Graphics& g)
{
    g.fillAll (Colours::white);
}

void MushraConfigComponent::resized()
{
    tabbedComponent->setBounds (8, 16, 784, 488);
    finishButton->setBounds (312, 536, 150, 24);
}

void MushraConfigComponent::buttonClicked (Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == finishButton)
    {
		Result res =  saveConfig();
		if ( res.failed() )
		{
			AlertWindow::showMessageBox( AlertWindow::WarningIcon, L"Error occured", res.getErrorMessage(), L"OK", this);
		}
		else
		{
			// close the program
			JUCEApplication::getInstance()->quit();
		}

    }
}

Result MushraConfigComponent::saveConfig()
{
	AudioDeviceManager::AudioDeviceSetup	audioDeviceSetup;

	deviceManager.getAudioDeviceSetup(audioDeviceSetup);

	ScopedPointer<XmlElement> audioDeviceState ( deviceManager.createStateXml() );

	/* create audioDeviceState when no changes were applied to the device settings */
	if (audioDeviceState == NULL)
	{
		audioDeviceState = createXmlSettings(deviceManager);
	}

	/* Check if audioDeviceState is complete */
	String attrString;

	attrString = "audioDeviceRate";
	if ( !audioDeviceState->hasAttribute( attrString ) )
		audioDeviceState->setAttribute(attrString, audioDeviceSetup.sampleRate );
	
	attrString = "audioDeviceBufferSize";
	if ( !audioDeviceState->hasAttribute(attrString ) )
		audioDeviceState->setAttribute( attrString, audioDeviceSetup.bufferSize );

	attrString = "audioDeviceInChans";
	if ( !audioDeviceState->hasAttribute( attrString ) )
		audioDeviceState->setAttribute( attrString,  audioDeviceSetup.inputChannels.toString(2, 1) );

	attrString = "audioDeviceOutChans";
	if ( !audioDeviceState->hasAttribute( attrString ) )
		audioDeviceState->setAttribute( attrString,  audioDeviceSetup.outputChannels.toString(2, 1) );

	/* add more attributes to audioDeviceState */
	audioDeviceState->setAttribute( "stimuliBufferSize", audioDeviceSettings.getStimuliBufferSize() );
	audioDeviceState->setAttribute( "stimuliCount", audioDeviceSettings.getStimuliCount() );
	audioDeviceState->setAttribute( "randomiseStimuli", audioDeviceSettings.getRandomised() );

	/* save subject names and stimuli folders */
	SubjectTableListBox* tmpModel = (SubjectTableListBox* ) table.getModel();
	XmlElement* subj = tmpModel->getTableData();
	XmlElement* tempChild;
	for (int i=0; i < tmpModel->getNumRows(); ++i)
	{
		tempChild = new XmlElement( *(subj->getChildElement(i)) ); // this will be deleted automatically by the parent
		audioDeviceState->addChildElement(tempChild);
	}

	/* update stimuli count parameter */
	tempChild = subj->getChildElement(0);
	if (tempChild == nullptr)
		return Result::fail( L"Wrong input or empty subjects list" );
	
	File firstStimDir( tempChild->getStringAttribute( L"stimuliDirectory") );

	if (firstStimDir.isDirectory())
	{
		Array<File> tmpDir;
		int trialsFound = firstStimDir.findChildFiles( tmpDir, File::findDirectories, false);
		int stimFound = -1;
		
		for (int i=0; i<trialsFound; ++i)
		{
			if (i==0)
				stimFound = tmpDir[i].getNumberOfChildFiles( File::findFiles, L"*.wav");
			else
			{
				if (tmpDir[i].getNumberOfChildFiles( File::findFiles, L"*.wav") != stimFound)
					return Result::fail( L"The number of stimuli in different trials should be the same" );
			}
		}

		audioDeviceState->setAttribute( "stimuliCount",stimFound );	
		if (stimFound == 0)
		{
			return Result::fail( L"No stimuli files were found in the specified directory" );	
		}
	}
	else
	{
		return Result::fail( L"Stimuli directory not found" );
	}




	File	settingsFile( settingsFileName );
	if (settingsFile == File::nonexistent)
	{
		return Result::fail( L"Settings file is absent" );;	
	}

	DBG( L"Settings file created:\t" + settingsFile.getFileName() );

	if	( !audioDeviceState->writeToFile ( settingsFile, String::empty ) )
	{
		return Result::fail( L"Unable to save settings" );
	}

	/* update settings */
	audioDeviceSettings.setXmlSettingsFile( settingsFile.getFullPathName() );

	deviceManager.closeAudioDevice();

	return Result::ok();
}

// ============================================================================================
XmlElement* MushraConfigComponent::createXmlSettings(AudioDeviceManager& adm)
{
	XmlElement* res;
	
	try
	{
		res = new XmlElement ("DEVICESETUP");
	}
	catch (std::bad_alloc)
	{
		return NULL;
	}

	AudioDeviceManager::AudioDeviceSetup	adSetup;
	adm.getAudioDeviceSetup(adSetup);

	res->setAttribute ("deviceType", adm.getCurrentAudioDeviceType());
	res->setAttribute ("audioOutputDeviceName", adSetup.outputDeviceName);
	res->setAttribute ("audioInputDeviceName", adSetup.inputDeviceName);
	res->setAttribute ("audioDeviceRate", adSetup.sampleRate);
	res->setAttribute ("audioDeviceBufferSize", adSetup.bufferSize);
	res->setAttribute ("audioDeviceInChans", adSetup.inputChannels.toString (2));
	res->setAttribute ("audioDeviceOutChans", adSetup.outputChannels.toString (2));

	return res;
}

