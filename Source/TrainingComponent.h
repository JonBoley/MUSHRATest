/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  4 May 2011 2:47:32 pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_TRAININGCOMPONENT_TRAININGCOMPONENT_B48F74C8__
#define __JUCER_HEADER_TRAININGCOMPONENT_TRAININGCOMPONENT_B48F74C8__

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
#include "guisettings.h"
#include "TestLauncher.h"
#include "ViewportComponent.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class TrainingComponent  : public Component,
                           public ButtonListener,
						   public ChangeBroadcaster
{
public:
    //==============================================================================
    TrainingComponent (DocumentWindow* parent);
    ~TrainingComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void checkExit();
	void setLauncher(TestLauncher* tl) {viewportComponent->setLauncher(tl);};
	ViewportComponent* getViewportComponent() {return viewportComponent;};
	//[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);


    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    DocumentWindow*		parentWindow;
	ViewportComponent*	viewportComponent;
	//[/UserVariables]

    //==============================================================================
    Viewport* viewport;
    GroupComponent* groupComponent;
    TextEditor* textEditor;
    TextButton* forwardButton;
    Label* label1;
    Label* label2;

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    TrainingComponent (const TrainingComponent&);
    const TrainingComponent& operator= (const TrainingComponent&);
};


//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

/* Document window to accomodate TrainingComponent */
class TrainingComponentWindow  : public DocumentWindow					
{
public:
    //==============================================================================
    TrainingComponentWindow() 
        : DocumentWindow (L"MUSHRA test - Training Part A",
                          Colours::lightgrey, 
						  DocumentWindow::closeButton,
                          true)
    {
        trainingComponent = new TrainingComponent(this);

        setContentOwned (trainingComponent, true);

        centreWithSize (getWidth(), getHeight());

        setVisible (false);


    }

    ~TrainingComponentWindow()
    {
		clearContentComponent();
	}

    //==============================================================================
    void closeButtonPressed()
    {
		JUCEApplication::getInstance()->systemRequestedQuit();
    }


private:
	TrainingComponent*	trainingComponent;
};

//[/MiscUserCode]


#endif   // __JUCER_HEADER_TRAININGCOMPONENT_TRAININGCOMPONENT_B48F74C8__
