
#ifndef __JUCER_HEADER_MUSHRAMAINCOMPONENT_MUSHRAMAINCOMPONENT_3D34C07C__
#define __JUCER_HEADER_MUSHRAMAINCOMPONENT_MUSHRAMAINCOMPONENT_3D34C07C__

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
#include "TestLauncher.h"

#include "guisettings.h"
#include "SubjectIDComponent.h"
#include "TrainingComponent.h"

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
	GUI Component for MUSHRA tests.
                                                                    //[/Comments]
*/
class MushraMainComponent  : public Component,
                             public SliderListener,
                             public ButtonListener,
							 public ChangeListener,
							 public Timer
{
public:
    //==============================================================================
    MushraMainComponent ();
    ~MushraMainComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void timerCallback(); 
	void changeListenerCallback (ChangeBroadcaster *objectThatHasChanged);
	void createButtons();
	//[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider* sliderThatWasMoved);
    void buttonClicked (Button* buttonThatWasClicked);
    bool keyPressed (const KeyPress& key);


    
	//==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	ScopedPointer<SubjectIDDocumentWindow>	subjectIDDocumentWindow;
	SubjectIDComponent*						subjectIDComponent;

	ScopedPointer<TrainingComponentWindow>	trainingComponentWindow;
	TrainingComponent*						trainingComponent;
	
	ScopedPointer<TooltipWindow>		tooltipWindow;

	ScopedPointer<TestLauncher>			testLauncher;
	ScopedPointer<AudioDeviceSettings>	audioDeviceSettings;
	
	int						stimCount;
	Trial*					tmpTest;
	bool					runningTraining;

	/* GUI members */
	int buttonWidth;
	int spacing;
	Array<TextButton*>		stimuliButtons;
	Array<Colour>			stimButtonColours;
	Array<Slider*>			ratingSliders;
    OwnedArray<Path>		internalPath;
	Array<Label*>			ratingLabels;
	StringArray				labelsText;

	String	tmpString;
	
	bool	saveResponses();
	bool	readResponses();
	void	clearResponses();
	void	selectStimulus(int ind);

	//[/UserVariables]

    //==============================================================================
    GroupComponent* testComponent;
    GroupComponent* groupComponent;
    Slider* playbackSlider;
    TextButton* playButton;
    TextButton* forwardButton;
    TextButton* backButton;
    ToggleButton* loopToggleButton;
    TextButton* referenceButton;
    Label* positionLabel;
    TextButton* nextButton;

	//==============================================================================
    // (prevent copy constructor and operator= being generated..)
    MushraMainComponent (const MushraMainComponent&);
    const MushraMainComponent& operator= (const MushraMainComponent&);
};


#endif   // __JUCER_HEADER_MUSHRAMAINCOMPONENT_MUSHRAMAINCOMPONENT_3D34C07C__
