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

#include "MushraMainComponent.h"

#define REF_BUTTON_W	600
#define REF_BUTTON_Y	40

#define BUTTON_X	32
#define BUTTON_Y	80
#define BUTTON_H	32
#define BUTTON_TOOLTIP	L"play stimulus "
#define	REL_BUTTON_SPACING	0.75f

#define SLIDER_X	32
#define SLIDER_Y	117
#define SLIDER_H	300
#define SLIDER_TOOLTIP	L"rate stimulus "
#define SLIDER_LABEL_H	30

#define	LINE_BG_COLOUR	Colour(0xff2a5ca5)
#define	LINE_COLOUR		Colours::cadetblue
#define LINE_W		600.0f
#define LINE_X		(float)SLIDER_X
#define LINE_Y		(float)SLIDER_Y + 10
#define LINE_SPACING	5


#define LABEL_SPACING	20
#define LABEL_X		(int)(LABEL_SPACING + LINE_X + LINE_W)
#define LABEL_Y		SLIDER_Y - 10
#define LABEL_W		72	
#define LABEL_H		24

//==============================================================================
MushraMainComponent::MushraMainComponent ()
    :	testComponent (0),
		groupComponent (0),
		playbackSlider (0),
		playButton (0),
		forwardButton (0),
		backButton (0),
		loopToggleButton (0),
		referenceButton (0),
		positionLabel (0),
		nextButton (0),
		subjectIDDocumentWindow(NULL),
		subjectIDComponent(NULL),
		trainingComponentWindow(NULL),
		trainingComponent(NULL),
		tooltipWindow(NULL),
		testLauncher(NULL),
		audioDeviceSettings(NULL),
		stimCount(0),
		tmpTest(NULL),
		runningTraining(false)
{  
    addAndMakeVisible (testComponent = new GroupComponent (L"new group",
                                                           L"  Test Stimuli  "));
    testComponent->setTextLabelPosition (Justification::centred);

    addAndMakeVisible (groupComponent = new GroupComponent (L"new group",
                                                            L"playback"));


    addAndMakeVisible (playbackSlider = new Slider (L"playback slider"));
    playbackSlider->setTooltip (L"current playback position");
    playbackSlider->setRange (0, SLIDER_MAXVALUE, 1);
    playbackSlider->setSliderStyle (Slider::ThreeValueHorizontal);
    playbackSlider->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
	playbackSlider->setMinValue(0, false, false);
	playbackSlider->setMaxValue(100, false, false);
	playbackSlider->setChangeNotificationOnlyOnRelease(true); 	
    playbackSlider->addListener (this);

    addAndMakeVisible (playButton = new TextButton (L"play button"));
    playButton->setTooltip (L"play / pause");
    playButton->setButtonText (L"pause");
    playButton->setConnectedEdges (Button::ConnectedOnLeft | Button::ConnectedOnRight);
    playButton->addListener (this);

    addAndMakeVisible (forwardButton = new TextButton (L"forward button"));
    forwardButton->setTooltip (L"go to the end");
    forwardButton->setButtonText (L"forward");
    forwardButton->setConnectedEdges (Button::ConnectedOnLeft);
    forwardButton->addListener (this);

    addAndMakeVisible (backButton = new TextButton (L"back button"));
    backButton->setTooltip (L"rewind to the beginning");
    backButton->setButtonText (L"back");
    backButton->setConnectedEdges (Button::ConnectedOnRight);
    backButton->addListener (this);

    addAndMakeVisible (loopToggleButton = new ToggleButton (L"loop toggle button"));
    loopToggleButton->setTooltip (L"play in a loop");
    loopToggleButton->setButtonText (L"loop");
    loopToggleButton->addListener (this);

    addAndMakeVisible (referenceButton = new TextButton (L"reference button"));
    referenceButton->setTooltip (L"play reference stimulus");
    referenceButton->setButtonText (L"reference");
	referenceButton->setColour(TextButton::buttonColourId, referenceButtonColour);	
    referenceButton->addListener (this);

    addAndMakeVisible (positionLabel = new Label (L"position label",
                                                  L"00:00.0"));
    positionLabel->setTooltip (L"current position");
    positionLabel->setFont (Font (15.0000f, Font::plain));
    positionLabel->setJustificationType (Justification::centredLeft);
    positionLabel->setEditable (false, false, false);
    positionLabel->setColour (TextEditor::textColourId, Colours::black);
    positionLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));


    addAndMakeVisible (nextButton = new TextButton (L"next button"));
    nextButton->setButtonText (L"next >");
	nextButton->setTooltip (L"proceed to the next trial");
    nextButton->addListener (this);

	audioDeviceSettings = new AudioDeviceSettings();



	File	xmlFile(  File::getSpecialLocation(File::userApplicationDataDirectory).getChildFile(L"MUSHRATest").getChildFile(L"settings.xml") );
	String	xmlSettingsFile( xmlFile.getFullPathName() );

	if ( !xmlFile.existsAsFile() )
	{
		AlertWindow::showMessageBox( AlertWindow::WarningIcon, L"Error occured", L"Unable to find settings file:\t" + xmlSettingsFile, L"OK", this);
		// close the program
		JUCEApplication::getInstance()->quit();
	}
	
	bool res = audioDeviceSettings->setXmlSettingsFile(xmlSettingsFile);

	if (!res)
	{	
		String errorMsg(audioDeviceSettings->getErrorString());
		errorMsg += "\n\nRun MUSHRAConfig to check the settings.";
		AlertWindow::showMessageBox( AlertWindow::WarningIcon, L"Error occured", errorMsg, L"OK", this);
		// close the program
		JUCEApplication::getInstance()->quit();
	}
	
	tooltipWindow = new TooltipWindow(this);

	/* create subject ID window */
	subjectIDDocumentWindow = new SubjectIDDocumentWindow();
	subjectIDComponent = ( SubjectIDComponent* ) subjectIDDocumentWindow->getContentComponent();


	if (! subjectIDComponent ->readSettings( audioDeviceSettings->getSubjects() ) )
	{
		// error message
	}
	else
	{
		/* add this component to subjectIDComponent list of ChangeListeners */
		subjectIDComponent->addChangeListener(this);
		subjectIDDocumentWindow->centreWithSize(subjectIDDocumentWindow->getWidth(), subjectIDDocumentWindow->getHeight());
		subjectIDDocumentWindow->setVisible(true);
		subjectIDDocumentWindow->runModalLoop();

	}

    setSize (800, 580);
}


MushraMainComponent::~MushraMainComponent()
{

	TextButton*	textButton;
	Slider*		slider;
	Label*		lbl;
	for (int i=0; i < stimCount ; i++)
	{
		textButton = stimuliButtons[i];
		slider = ratingSliders[i];

		deleteAndZero ( textButton );
		deleteAndZero ( slider );
	} 	
	
	for (int i=0; i<ratingLabels.size(); i++)
	{
		lbl = ratingLabels[i];
		deleteAndZero ( lbl );
	}

	stimuliButtons.clear();
	stimButtonColours.clear();
	ratingLabels.clear();

    deleteAndZero (testComponent);
    deleteAndZero (groupComponent);
    deleteAndZero (playbackSlider);
    deleteAndZero (playButton);
    deleteAndZero (forwardButton);
    deleteAndZero (backButton);
    deleteAndZero (loopToggleButton);
    deleteAndZero (referenceButton);
    deleteAndZero (positionLabel);
    deleteAndZero (nextButton);

	internalPath.clear();
	labelsText.clear();

	/* trainingComponentWindow; subjectIDDocumentWindow; tooltipWindow; audioDeviceSettings;	testLauncher
	   get deleted here by ScopedPointer */

}

//==============================================================================
void MushraMainComponent::paint (Graphics& g)
{
    g.fillAll (Colours::white);

	Path* tmpPath;
	for (int i=0; i<internalPath.size(); i++)
	{
		tmpPath = internalPath[i];
		g.setColour ( LINE_BG_COLOUR );
		g.fillPath ( *tmpPath );
		g.setColour ( LINE_COLOUR );
		g.strokePath ( *tmpPath, PathStrokeType (2.0000f));	
	}
}

void MushraMainComponent::resized()
{
    testComponent->setBounds (8, 9, 784, 456);
    groupComponent->setBounds (8, 466, 784, 108);

	playbackSlider->setBounds (104, 491, 560, 24);
    playButton->setBounds (360, 536, 72, 24);
    forwardButton->setBounds (432, 536, 104, 24);
    backButton->setBounds (256, 536, 104, 24);
    loopToggleButton->setBounds (32, 491, 56, 24);

    positionLabel->setBounds (688, 491, 80, 24);
    nextButton->setBounds (344, 432, 96, 24);


	int lineSpacing = (SLIDER_H - LINE_SPACING - SLIDER_LABEL_H)/(internalPath.size() - 1);
	for (int i=0; i<internalPath.size(); i++)
	{
		internalPath[i]->clear();
		internalPath[i]->startNewSubPath (LINE_X, LINE_Y + i*lineSpacing );
		internalPath[i]->lineTo (LINE_X + LINE_W, LINE_Y + i*lineSpacing);
		internalPath[i]->closeSubPath();
	}    

	for (int i=0; i<ratingLabels.size(); i++)
	{
		ratingLabels[i]->setBounds (LABEL_X, (int)(LABEL_Y + i*lineSpacing), LABEL_W, LABEL_H);
	} 
	
	buttonWidth = (int)ceil( (float) REF_BUTTON_W /(stimCount + (stimCount - 1)*(1 - REL_BUTTON_SPACING)/REL_BUTTON_SPACING) );
	spacing =  (int) (buttonWidth/REL_BUTTON_SPACING);
	for (int i=0; i < stimCount ; i++)
	{
		stimuliButtons[i]->setBounds (BUTTON_X + spacing*i, BUTTON_Y, buttonWidth, BUTTON_H);
		ratingSliders[i]->setBounds (SLIDER_X + spacing*i, SLIDER_Y, buttonWidth, SLIDER_H);
	}

    referenceButton->setBounds (BUTTON_X, REF_BUTTON_Y, REF_BUTTON_W, BUTTON_H);	
		
}

void MushraMainComponent::sliderValueChanged (Slider* sliderThatWasMoved)
{
    if (sliderThatWasMoved == playbackSlider)
    {
		float curPosition;
		
		curPosition = (float) playbackSlider->getMinValue() / SLIDER_MAXVALUE;
		testLauncher->setFragmentStartPosition(curPosition);

		curPosition = (float) playbackSlider->getMaxValue() / SLIDER_MAXVALUE;
		testLauncher->setFragmentEndPosition(curPosition);

		curPosition = (float) playbackSlider->getValue() / SLIDER_MAXVALUE;
		testLauncher->setPosition(curPosition);
    }
}

void MushraMainComponent::buttonClicked (Button* buttonThatWasClicked)
{

    if (buttonThatWasClicked == playButton)
    {
		if ( playButton->getButtonText() == L"play" )
		{
			testLauncher->resume();
			playButton->setButtonText( L"pause" );
		}
		else
		{
			testLauncher->pause();
			playButton->setButtonText( L"play" );		
		}
    }
    else if (buttonThatWasClicked == nextButton)
    {
		/* deselect all stimuli */
		selectStimulus(-1);
		referenceButton->setColour(TextButton::buttonColourId, referenceButtonColour);
		
		if ( saveResponses() )	
		{
			
			/* stop current playback */
			testLauncher->stop();
			stopTimer();

			/* update interface components	*/
			playbackSlider->setMinValue(0, false, false, false);
			playbackSlider->setMaxValue(SLIDER_MAXVALUE, false, false, false);
			playbackSlider->setValue(0, false, false);
			playButton->setButtonText( L"pause") ;
			sliderValueChanged(playbackSlider);

			loopToggleButton->setToggleState(false, false);

			/* part B of training session */
			bool shouldContinue = true;
			if (runningTraining)
			{
				AlertWindow::showMessageBox( AlertWindow::InfoIcon, L"Training complete", L"This is the end of training. Click forward to start the test.", L"forward", this);
				nextButton->setButtonText(L"next >");
				
				/* clear the results of the training trail */
				clearResponses(); 
				
				/* set the sliders to default */
				readResponses();

				/* go to the first trial */
				testLauncher->goToTrial(0);	

				runningTraining = false;
			}
			else
			{
				/* Save temporary file */
				testLauncher->saveTempResults();
				
				/* Load audio data and go to the next test */				
				shouldContinue = testLauncher->goForward();

				if (testLauncher->lastError.isNotEmpty())
				{
					AlertWindow::showMessageBox( AlertWindow::WarningIcon, L"Error", testLauncher->lastError, L"OK", this);
					return;
				}

				/* save results if the last trial is finished */
				if ( !shouldContinue )
				{			
					/* save results to file */
					if (testLauncher->saveResults())
					{
						// notify the user if results were saved in a differt directory
						if ( testLauncher->getResultsDir().isNotEmpty() )
						{
							String msg(L"Results were saved in " + testLauncher->getResultsDir());
						    AlertWindow::showMessageBox( AlertWindow::InfoIcon, L"Information", msg, L"OK", this);
						}
						// close the program
						((DocumentWindow*) getParentComponent())->closeButtonPressed();
					}
					else
					{
						// error message - results not saved
						AlertWindow::showMessageBox( AlertWindow::WarningIcon, L"Error", testLauncher->lastError, L"OK", this);
					}
					return;
				}
			}
			
			/* update playback parameters */
			sliderValueChanged(playbackSlider);

			/* start the new playback and the timer */
			if (!testLauncher->start(audioDeviceSettings->getXmlDeviceSettings()))
			{
				AlertWindow::showMessageBox( AlertWindow::WarningIcon, L"Error", testLauncher->lastError, L"OK", this);
			}
			else
			{
				startTimer(500);
			}

			/* change test title */
			testComponent->setText(L"   Trial " + String(testLauncher->getCurrentTestIndex()+1) +L" of " + String(testLauncher->getTrialsCount()) + L": " + testLauncher->getCurrentTestName() + L"   ");
			
			if (testLauncher->getCurrentTestIndex() == testLauncher->getTrialsCount() - 1)
			{
				/* update button text if the last test */
				nextButton->setButtonText(L"done");
			}
	
			/* set the sliders to default */
			readResponses();
		}
		else
		{
			// error message - incorect input
			AlertWindow::showMessageBox( AlertWindow::WarningIcon, L"Error", L"Incorrect input.", L"OK", this);
			return;
		}
	}

    else if (buttonThatWasClicked == loopToggleButton)
    {
		testLauncher->setPlayLoop( loopToggleButton->getToggleState() );
    }
    else if (buttonThatWasClicked == referenceButton)
    {
		if (!testLauncher->isRunning())
		{
			sliderValueChanged(playbackSlider);
			if (!testLauncher->start(audioDeviceSettings->getXmlDeviceSettings()))
			{
				AlertWindow::showMessageBox( AlertWindow::WarningIcon, L"Error", testLauncher->lastError, L"OK", this);
				return;
			}			
			testLauncher->resume();
			startTimer(500);
		}
		selectStimulus(-1);
		testLauncher->switchStimulus( testLauncher->getCurrentTest()->refIndex );
    }
    else if (buttonThatWasClicked == backButton)
    {
		playbackSlider->setValue(playbackSlider->getValue() - SLIDER_STEP, true, false);
    }
    else if (buttonThatWasClicked == forwardButton)
    {
		playbackSlider->setValue(playbackSlider->getValue() + SLIDER_STEP, true, false);
    }

	for (int i=0; i<stimCount; i++)
	{
		if (buttonThatWasClicked == stimuliButtons[i])
		{
			if (!testLauncher->isRunning())
			{
				sliderValueChanged(playbackSlider);
				if (!testLauncher->start(audioDeviceSettings->getXmlDeviceSettings()))
				{
					AlertWindow::showMessageBox( AlertWindow::WarningIcon, L"Error", testLauncher->lastError, L"OK", this);
					return;
				}			
				testLauncher->resume();
				startTimer(500);
			}
			selectStimulus(i);
			testLauncher->switchStimulus( testLauncher->getCurrentTest()->filesOrder[i]);

			testLauncher->dbgOut( L"playing stimulus " + String(i) + L" \t" + testLauncher->getCurrentTest()->soundFiles[ testLauncher->getCurrentTest()->filesOrder[i] ] );

			break;
		}
	}

}

bool MushraMainComponent::keyPressed (const KeyPress& )
{
    return false;  // Return true if the handler uses this key event, or false to allow it to be passed-on.
}



void MushraMainComponent::changeListenerCallback (ChangeBroadcaster *objectThatHasChanged)
{
	if ( objectThatHasChanged == subjectIDComponent)
	{
		stimCount = audioDeviceSettings->getStimuliCount();
		testLauncher = new TestLauncher( audioDeviceSettings );

		String subID = subjectIDComponent->getSubjectID();
		testLauncher->setSubjectID( subID );
		
		int subjectIndex = audioDeviceSettings->getSubjects()->indexOf(subID);

		testLauncher->init( audioDeviceSettings->getStimuliDirectory(subjectIndex) );

		/* check for errors */
		if (testLauncher->lastError.isNotEmpty())
		{
			stimCount = 0;
			testLauncher->dbgOut(testLauncher->lastError);
			AlertWindow::showMessageBox( AlertWindow::WarningIcon, L"Error occured", testLauncher->lastError, L"OK", this);
			// close the program
			JUCEApplication::getInstance()->quit();
			return;
		}	
		
		/* create the controls */
		this->createButtons();

		/* create training component window */
		trainingComponentWindow = new TrainingComponentWindow();
		trainingComponent = ( TrainingComponent* ) trainingComponentWindow->getContentComponent();	
		trainingComponent->setLauncher(testLauncher);	
		trainingComponent->addChangeListener(this);
		trainingComponent->getViewportComponent()->addChangeListener(this);

		if (subjectIDComponent->getSkipState())
		{
			/* update playback parameters */
			float curPosition;

			curPosition = (float) playbackSlider->getMinValue() / SLIDER_MAXVALUE;
			testLauncher->setFragmentStartPosition(curPosition);

			curPosition = (float) playbackSlider->getMaxValue() / SLIDER_MAXVALUE;
			testLauncher->setFragmentEndPosition(curPosition);

			curPosition = (float) playbackSlider->getValue() / SLIDER_MAXVALUE;
			testLauncher->setPosition(curPosition);			
		}
		else
		{
			/* Show training component */			
			trainingComponentWindow->centreWithSize(trainingComponentWindow->getWidth(), trainingComponentWindow->getHeight());
			trainingComponentWindow->setVisible(true);			
			trainingComponent->getViewportComponent()->addStimuliButtons();			
			trainingComponentWindow->runModalLoop();

		}
	}
	if ( objectThatHasChanged == trainingComponent->getViewportComponent() )
	{
		/* play training stimulus */
		int buttonId = trainingComponent->getViewportComponent()->getSelectedStimulus();
		int trialIndex = div( buttonId, stimCount+1).quot;
		int stimIndex = div(buttonId, stimCount+1).rem;
		
		testLauncher->stop();
		
		/* go to the required trial */
		if ( testLauncher->getCurrentTestIndex() != trialIndex )
		{
			testLauncher->goToTrial(trialIndex);
		}

		if (stimIndex == 0) 
		{
			/* reference was chosen */
			stimIndex = testLauncher->getCurrentTest()->refIndex;
		}
		else
		{
			stimIndex =  stimIndex--;
		}

		testLauncher->dbgOut( L"playing stimulus " + String( stimIndex) + L" \t" + testLauncher->getCurrentTest()->soundFiles[stimIndex] );


		bool res = testLauncher->start(audioDeviceSettings->getXmlDeviceSettings());
		if ( !res )
		{
			AlertWindow::showMessageBox( AlertWindow::WarningIcon, L"Error occured", testLauncher->lastError, L"OK", this);
			// close the program
			JUCEApplication::getInstance()->quit();
		}
	
		testLauncher->switchStimulus( stimIndex );

		/* set playback controls */
		testLauncher->setFragmentStartPosition(0.0);
		testLauncher->setFragmentEndPosition(1.0);
		testLauncher->setPosition(0.0);
	}	
	else if ( objectThatHasChanged == trainingComponent )
	{
		/* start part B of training */
		testLauncher->dbgOut( L"start part B of training  " );
		
		testLauncher->stop();
		runningTraining = true;
		testComponent->setText( L"Part B of training phase" );
		nextButton->setButtonText( L"finish training" );
		startTimer(500);
	}
}


bool MushraMainComponent::saveResponses()
{
	tmpTest = testLauncher->getCurrentTest();
	if (tmpTest != NULL)		
	{
		for (int i=0; i<stimCount; i++)
		{
			tmpTest->responses.set( i, int(ratingSliders[i]->getValue()) );
		}
		return tmpTest->isValid();
	}
	else
	{
		return false;
	}
}

void MushraMainComponent::clearResponses()
{
	tmpTest = testLauncher->getCurrentTest();
	if (tmpTest != NULL)		
	{
		for (int i=0; i<stimCount; i++)
		{
			tmpTest->responses.set( i, 0 );
		}
	}
}


bool MushraMainComponent::readResponses()
{
	tmpTest = testLauncher->getCurrentTest(); 
	if (tmpTest != NULL)		
	{	
		for (int i=0; i<stimCount; i++)
		{
			ratingSliders[i]->setValue( tmpTest->responses[i] , false);
		}
		
		/* update title */
		tmpString = L"   Trial " + String(testLauncher->getCurrentTestIndex()+1) +  L" of " + String(testLauncher->getTrialsCount()) + L": " + testLauncher->getCurrentTestName() + L"   ";
		testComponent->setText(tmpString); 
		
		return true;
	}
	else
	{
		return false;
	}
}

void MushraMainComponent::selectStimulus(int ind)
{
	// deactivate all
	for (int i=0; i<stimCount; i++)
	{
		stimuliButtons[i]->setColour(TextButton::buttonColourId, stimButtonColours[i]);
		ratingSliders[i]->setEnabled(false);
	}
	
	// activate selected stimuli 
	if ((ind > -1) && (ind < stimuliButtons.size() ))
	{
		stimuliButtons[ind]->setColour(TextButton::buttonColourId, activeButtonColour);
		ratingSliders[ind]->setEnabled(true);
		referenceButton->setColour(TextButton::buttonColourId, referenceButtonColour);
	}
	else
	{
		referenceButton->setColour(TextButton::buttonColourId, activeButtonColour);	
	}
}

void MushraMainComponent::timerCallback() 
{
	if (testLauncher->isRunning())
	{
		double audioPosition = testLauncher->getCurrentPosition();
		int curPosition = (int) floor(100*audioPosition/testLauncher->getTotalLength());
		playbackSlider->setValue(curPosition, false, false);
		
		tmpString = String::formatted( L"%3.1f", audioPosition/audioDeviceSettings->getSampleRate() ); 
		positionLabel->setText(tmpString, false);
	}
}


void MushraMainComponent::createButtons()
{
	/* change the button text if only one trial */
	if (testLauncher->getTrialsCount() == 1)
	{
		nextButton->setButtonText(L"done");
	}

	/* create buttons and sliders */
	juce_wchar		letter;
	String			letterStr;
	Colour			c;
	for (int i=0; i < stimCount ; i++)
	{
		letter = 'A'+ i;
		letterStr = String::charToString(letter);
		
		stimuliButtons.add( new TextButton ( L"button" + letterStr ) );
		stimButtonColours.add( Colours::findColourForName( String(stimuliButtonColours[i]).trimEnd(), Colours::antiquewhite ) );
		addAndMakeVisible ( stimuliButtons[i] );
		stimuliButtons[i]->setTooltip ( BUTTON_TOOLTIP + letterStr);
		stimuliButtons[i]->setButtonText ( letterStr );
		stimuliButtons[i]->setColour( TextButton::buttonColourId, stimButtonColours[i] );	
		stimuliButtons[i]->addListener (this);

		ratingSliders.add( new Slider ( L"slider" + letterStr ) );
		addAndMakeVisible ( ratingSliders[i] );
		ratingSliders[i]->setTooltip ( SLIDER_TOOLTIP + letterStr );
		ratingSliders[i]->setRange (0, 100, 1);
		ratingSliders[i]->setSliderStyle (Slider::LinearVertical);
		ratingSliders[i]->setTextBoxStyle (Slider::TextBoxBelow, false, 80, 20);
		ratingSliders[i]->setEnabled(false);
		ratingSliders[i]->addListener (this);
	}

	/* create labels */
	labelsText.add( L"Excellent" );
	labelsText.add( L"Good" );
	labelsText.add( L"Fair" );
	labelsText.add( L"Poor" );
	labelsText.add( L"Bad" );	
	
	/* create lines */
	for (int i=0; i<labelsText.size(); i++)
	{
		internalPath.add(new Path);
		ratingLabels.add( new Label ( L"label" + labelsText[i], labelsText[i] ) );
		addAndMakeVisible( ratingLabels[i] ); 
		ratingLabels[i]->setFont (Font (15.0000f, Font::plain));
		ratingLabels[i]->setJustificationType (Justification::centredLeft);
		ratingLabels[i]->setEditable (false, false, false);
		ratingLabels[i]->setColour (TextEditor::textColourId, Colours::black);
		ratingLabels[i]->setColour (TextEditor::backgroundColourId, Colour (0x0));
	}	
	
	testComponent->setText(L"   Trial 1 of " + String(testLauncher->getTrialsCount()) + L": " + testLauncher->getCurrentTestName() + L"   ");
	
	/* update the component */
	this->resized();

}
