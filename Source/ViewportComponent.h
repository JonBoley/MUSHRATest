#ifndef __JUCER_HEADER_VIEWPORTCOMPONENT_VIEWPORTCOMPONENT_AB20388D__
#define __JUCER_HEADER_VIEWPORTCOMPONENT_VIEWPORTCOMPONENT_AB20388D__

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
#include "guisettings.h"
#include "TestLauncher.h"

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class ViewportComponent  : public Component,
						   public ButtonListener,
						   public ChangeBroadcaster
{
public:
    //==============================================================================
    ViewportComponent ();
    ~ViewportComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	void	setLauncher(TestLauncher* tl) {testLauncher = tl;};
	void	addStimuliButtons();
	void	removeStimuliButtons();	
	void	selectStimulus(int ind);
	int		getSelectedStimulus() {return selectedStimulusIndex;};
	
	//[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);

    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    TestLauncher* testLauncher;

	Array<TextButton*>		stimuliButtons;
	Array<Colour>			stimButtonColours;

	int		stimCount;
	int		trialCount;
	int		selectedStimulusIndex;
	
	Viewport*				parentViewport;
	//[/UserVariables]

    //==============================================================================


    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    ViewportComponent (const ViewportComponent&);
    const ViewportComponent& operator= (const ViewportComponent&);
};


#endif   // __JUCER_HEADER_VIEWPORTCOMPONENT_VIEWPORTCOMPONENT_AB20388D__
