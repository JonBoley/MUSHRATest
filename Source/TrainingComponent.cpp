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

#include "TrainingComponent.h"

#define DESCRIPTION_TEXT		L"This is part A of the training phase. In this part you will become familiar with all the sound excerpts under test and their quality ranges. You can click on different buttons to listen to all stimuli including the reference."

//==============================================================================
TrainingComponent::TrainingComponent (	DocumentWindow*	parent )
    : Component (L"Training Phase"),
      viewport (0),
      groupComponent (0),
      textEditor (0),
      forwardButton (0),
      label1 (0),
      label2 (0),
	  parentWindow(parent)
{
    addAndMakeVisible (viewport = new Viewport (L"new viewport"));
    viewport->setScrollBarsShown (true, false);

    addAndMakeVisible (groupComponent = new GroupComponent (L"new group",
                                                            L" Stimuli "));

    addAndMakeVisible (textEditor = new TextEditor (L"description text editor"));
    textEditor->setMultiLine (true);
    textEditor->setReturnKeyStartsNewLine (false);
    textEditor->setReadOnly (true);
    textEditor->setScrollbarsShown (true);
    textEditor->setCaretVisible (true);
    textEditor->setPopupMenuEnabled (false);
    textEditor->setText (DESCRIPTION_TEXT);

    addAndMakeVisible (forwardButton = new TextButton (L"forward button"));
    forwardButton->setTooltip (L"proceed to part B");
    forwardButton->setButtonText (L"forward");
    forwardButton->addListener (this);

    addAndMakeVisible (label1 = new Label (L"label1",
                                           L"Reference stimuli\n"));
    label1->setFont (Font (15.0000f, Font::plain));
    label1->setJustificationType (Justification::centredLeft);
    label1->setEditable (false, false, false);
    label1->setColour (TextEditor::textColourId, Colours::black);
    label1->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (label2 = new Label (L"label2",
                                           L"Stimuli under test"));
    label2->setFont (Font (15.0000f, Font::plain));
    label2->setJustificationType (Justification::centredLeft);
    label2->setEditable (false, false, false);
    label2->setColour (TextEditor::textColourId, Colours::black);
    label2->setColour (TextEditor::backgroundColourId, Colour (0x0));

    viewportComponent = new ViewportComponent();
	viewport->setViewedComponent(viewportComponent);

    setSize (800, 600);
}

TrainingComponent::~TrainingComponent()
{
	removeAllChangeListeners();
    deleteAndZero (viewport);
    deleteAndZero (groupComponent);
    deleteAndZero (textEditor);
    deleteAndZero (forwardButton);
    deleteAndZero (label1);
    deleteAndZero (label2);
}

//==============================================================================
void TrainingComponent::paint (Graphics& g)
{
    g.fillAll (Colours::white);
}

void TrainingComponent::resized()
{
    viewport->setBounds (48, 190, 704, 320);
    groupComponent->setBounds (48, 144, 688, 368);
    textEditor->setBounds (56, 64, 688, 48);
    forwardButton->setBounds (312, 536, 150, 24);
    label1->setBounds (64, 168, 150, 24);
    label2->setBounds (328, 168, 150, 24);
}

void TrainingComponent::buttonClicked (Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == forwardButton)
    {
		checkExit();
    }
}

void  TrainingComponent::checkExit()
{
	if (parentWindow->isCurrentlyModal())
	{
		parentWindow->exitModalState(EXITNOERROR);
		parentWindow->removeFromDesktop();
		sendChangeMessage();
	}
}
