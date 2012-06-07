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

#include "SubjectsTabComponent.h"

//==============================================================================
SubjectsTabComponent::SubjectsTabComponent (TableListBox* t)
	: table(t),
      addButton (0),
      delButton (0)
{
    
	subjectTableListBox = (SubjectTableListBox* ) table->getModel();
	
	addAndMakeVisible (table);

    addAndMakeVisible (addButton = new TextButton (L"add button"));
    addButton->setButtonText (L"Add subject");
    addButton->addListener (this);

    addAndMakeVisible (delButton = new TextButton (L"delete button"));
    delButton->setButtonText (L"Delete");
    delButton->addListener (this);

}

SubjectsTabComponent::~SubjectsTabComponent()
{

    deleteAndZero (addButton);
    deleteAndZero (delButton);
}

//==============================================================================
void SubjectsTabComponent::paint (Graphics& g)
{
    g.fillAll ( Colours::aliceblue);
}

void SubjectsTabComponent::resized()
{
	int pw = getWidth();
	int ph = getHeight();
	int border = 20;
	int buttonW = 88;
	int buttonH = 24;
	int buttonXSpace = 24;
	int buttonYSpace = 2*border+buttonH;

	int y = ph-border-buttonYSpace;
	
	table->setBounds (border, border, pw-2*border, y);
	y += 2*border;
    addButton->setBounds (border, y, buttonW, buttonH);
    delButton->setBounds (border + buttonW + buttonXSpace, y, buttonW, buttonH);

}

void SubjectsTabComponent::buttonClicked (Button* buttonThatWasClicked)
{

    if (buttonThatWasClicked == addButton)
    {
		subjectTableListBox->addSubject();
    }
    else if (buttonThatWasClicked == delButton)
    {
		subjectTableListBox->deleteSubject();
    }

}


