
#ifndef __JUCER_HEADER_DIALOGWINCOMPONENT_DIALOGWINCOMPONENT_63E23E8E__
#define __JUCER_HEADER_DIALOGWINCOMPONENT_DIALOGWINCOMPONENT_63E23E8E__

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class DialogWinComponent  : public Component
{
public:
    //==============================================================================
    DialogWinComponent ();
    ~DialogWinComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();



    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================


    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    DialogWinComponent (const DialogWinComponent&);
    const DialogWinComponent& operator= (const DialogWinComponent&);
};



/* Document window to accomodate TrainingComponent */
class ProgressWindow  : public DocumentWindow					
{
public:
    //==============================================================================
    ProgressWindow() 
        : DocumentWindow ( L"Loading stimuli into memory",
                          Colours::lightgrey, 
						  DocumentWindow::closeButton,
                          true)
    {

        setContentOwned (new DialogWinComponent(), true);

        centreWithSize (getWidth(), getHeight());

        setVisible (false);


    }

    ~ProgressWindow()
    {
		clearContentComponent();
	}

    //==============================================================================
    void closeButtonPressed()
    {
	// ignore
    }

};


#endif   // __JUCER_HEADER_DIALOGWINCOMPONENT_DIALOGWINCOMPONENT_63E23E8E__
