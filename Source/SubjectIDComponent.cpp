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

#include "SubjectIDComponent.h"

//==============================================================================
SubjectIDComponent::SubjectIDComponent (DocumentWindow*	parent)
    : comboBox (0),
      textButton (0),
      skipToggleButton (0),
	  skipIntro(false),
      label (0),
	  parentWindow(parent)
{
    addAndMakeVisible (comboBox = new ComboBox (L"name combo box"));
    comboBox->setTooltip (L"Select your ID from the list or type it");
    comboBox->setEditableText (false);
    comboBox->setJustificationType (Justification::centredLeft);
    comboBox->setTextWhenNothingSelected (String::empty);
    comboBox->setTextWhenNoChoicesAvailable (L"(empty)");
    comboBox->addListener (this);

    addAndMakeVisible (textButton = new TextButton (L"new button"));
    textButton->setButtonText (L"forward");
    textButton->addListener (this);

    addAndMakeVisible (skipToggleButton = new ToggleButton (L"skip toggle button"));
    skipToggleButton->setTooltip (L"Check this box if you wish to skip stimuli and interface introduction section");
    skipToggleButton->setButtonText (L"skip intoduction screen");
    skipToggleButton->addListener (this);

    addAndMakeVisible (label = new Label (L"subject label",
                                          L"select your ID:"));
    label->setFont (Font (15.0000f, Font::plain));
    label->setJustificationType (Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x0));

    setSize (400, 200);
}

SubjectIDComponent::~SubjectIDComponent()
{
	removeAllChangeListeners();

    deleteAndZero (comboBox);
    deleteAndZero (textButton);
    deleteAndZero (skipToggleButton);
    deleteAndZero (label);
}

//==============================================================================
void SubjectIDComponent::paint (Graphics& g)
{
    g.fillAll (Colours::white);
}

void SubjectIDComponent::resized()
{
    comboBox->setBounds (48, 48, 304, 24);
    textButton->setBounds (128, 152, 150, 24);
    skipToggleButton->setBounds (48, 96, 150, 24);
    label->setBounds (48, 24, 150, 24);
}

void SubjectIDComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == comboBox)
    {
    }
}

void SubjectIDComponent::buttonClicked (Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == textButton)
    {
		checkExit();
    }
    else if (buttonThatWasClicked == skipToggleButton)
    {
    }
}


void  SubjectIDComponent::checkExit()
{
	if (comboBox->getSelectedId() >0 )
	{
		subjectID = comboBox->getText();
		skipIntro = skipToggleButton->getToggleState();
		if (parentWindow->isCurrentlyModal())
		{
			parentWindow->exitModalState(EXITNOERROR);
			parentWindow->removeFromDesktop();
			sendChangeMessage();
		}
	}
}


bool SubjectIDComponent::readSettings(StringArray* names)
{
	if (names->size() == 0)
		return false;
	
	for (int i=0; i<names->size(); i++)
	{
		comboBox->addItem( names->operator [](i), i+1 );
	}
	comboBox->setSelectedId(1, true);

	return true;

}
