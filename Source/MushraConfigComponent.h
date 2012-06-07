#ifndef __JUCER_HEADER_MUSHRACONFIGCOMPONENT_MUSHRACONFIGCOMPONENT_38E8704B__
#define __JUCER_HEADER_MUSHRACONFIGCOMPONENT_MUSHRACONFIGCOMPONENT_38E8704B__

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
#include "TestLauncher.h"
#include "guisettings.h"
#include "SubjectTableListBox.h"
#include "SubjectsTabComponent.h"

#define	MAXNUMBEROFDEVICECHANNELS	64

class SubjectTableListBox;
class SubjectsTabComponent;
//[/Headers]


//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class MushraConfigComponent  : public Component,
                               public ButtonListener
{
public:
    //==============================================================================
    MushraConfigComponent ();
    ~MushraConfigComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	Result saveConfig();
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);



    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	AudioDeviceManager				deviceManager;
	AudioDeviceSelectorComponent*	deviceSelector;

	AudioDeviceSettings				audioDeviceSettings;
	String							settingsFileName;

	SubjectTableListBox*			subjectTableListBox;
	TableListBox					table;
	SubjectsTabComponent*			subjectsTabComponent;
    //[/UserVariables]

    //==============================================================================
    TabbedComponent* tabbedComponent;
    TextButton* finishButton;

	XmlElement* createXmlSettings(AudioDeviceManager& adm);	

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
   // MushraConfigComponent (const MushraConfigComponent&);
  //  const MushraConfigComponent& operator= (const MushraConfigComponent&);

	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MushraConfigComponent);
};


#endif   // __JUCER_HEADER_MUSHRACONFIGCOMPONENT_MUSHRACONFIGCOMPONENT_38E8704B__
