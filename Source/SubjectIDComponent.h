/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  3 May 2011 12:03:57 pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_SUBJECTIDCOMPONENT_SUBJECTIDCOMPONENT_7D7868F__
#define __JUCER_HEADER_SUBJECTIDCOMPONENT_SUBJECTIDCOMPONENT_7D7868F__

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
#include "guisettings.h"

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class SubjectIDComponent  : public Component,
                            public ComboBoxListener,
                            public ButtonListener,
							public ChangeBroadcaster
{
public:
    //==============================================================================
    SubjectIDComponent (DocumentWindow* parent);
    ~SubjectIDComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    bool	readSettings(StringArray* names);
	String	getSubjectID() { return subjectID; };
	String  getStimDirectory();
	bool	getSkipState() { return skipIntro; };
	void	checkExit();
	void	initiateLoading();

	//[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);
    void buttonClicked (Button* buttonThatWasClicked);


    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    String	fileName;
	String	subjectID;
	bool	skipIntro;
	DocumentWindow*	parentWindow;
    //[/UserVariables]

    //==============================================================================
    ComboBox* comboBox;
    TextButton* textButton;
    ToggleButton* skipToggleButton;
    Label* label;

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    SubjectIDComponent (const SubjectIDComponent&);
    const SubjectIDComponent& operator= (const SubjectIDComponent&);
};

/* Document window to accomodate SubjectIDComponent */
class SubjectIDDocumentWindow  : public DocumentWindow					
{
public:
    //==============================================================================
    SubjectIDDocumentWindow() 
        : DocumentWindow (L"MUSHRA test - Subject Identification",
                          Colours::lightgrey, 
						  DocumentWindow::closeButton,
                          true)
    {

        subjectIDComponent = new SubjectIDComponent(this);

		setContentOwned (subjectIDComponent, true);

        centreWithSize (getWidth(), getHeight());

        setVisible (false);
    }

    ~SubjectIDDocumentWindow()
    {
		clearContentComponent();	
	}

    //==============================================================================
    void closeButtonPressed()
    {
		//subjectIDComponent->checkExit();
		JUCEApplication::getInstance()->systemRequestedQuit();
    }


private:
	SubjectIDComponent*	subjectIDComponent;
};



#endif   // __JUCER_HEADER_SUBJECTIDCOMPONENT_SUBJECTIDCOMPONENT_7D7868F__
