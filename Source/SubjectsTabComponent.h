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

*/#ifndef __JUCER_HEADER_SUBJECTSTABCOMPONENT_SUBJECTSTABCOMPONENT_CCCEC8FA__
#define __JUCER_HEADER_SUBJECTSTABCOMPONENT_SUBJECTSTABCOMPONENT_CCCEC8FA__

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
#include "SubjectTableListBox.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class SubjectsTabComponent  : public Component,
                              public ButtonListener
{
public:
    //==============================================================================
    SubjectsTabComponent (TableListBox* t);
    ~SubjectsTabComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);



    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
	TableListBox* table;
    TextButton* addButton;
    TextButton* delButton;

	SubjectTableListBox* subjectTableListBox;

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
   // SubjectsTabComponent (const SubjectsTabComponent&);
   // const SubjectsTabComponent& operator= (const SubjectsTabComponent&);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SubjectsTabComponent);
};


#endif   // __JUCER_HEADER_SUBJECTSTABCOMPONENT_SUBJECTSTABCOMPONENT_CCCEC8FA__
