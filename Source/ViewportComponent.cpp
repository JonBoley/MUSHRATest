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

#include "ViewportComponent.h"

#define BUTTON_SPACING 48
#define ROW_SPACING	50

#define REFBUTTON_X	20
#define REFBUTTON_Y	20
#define REFBUTTON_W	200
#define REFBUTTON_H	36

#define BUTTON_X	REFBUTTON_X + REFBUTTON_W + BUTTON_SPACING
#define BUTTON_Y	20
#define BUTTON_W	36
#define BUTTON_H	36

#define REF_TOOLTIP_TEXT		L"Reference for trial "
#define BUTTON_TOOLTIP_TEXT1	L"Test stimulus "
#define BUTTON_TOOLTIP_TEXT2	L" for trial "

//==============================================================================
ViewportComponent::ViewportComponent ()
    : Component (L"Viewport Component"),
	  testLauncher(NULL),
	  stimCount(0),
	  trialCount(0),
	  selectedStimulusIndex(-1)
{
    setSize (600, 400);
	parentViewport = (Viewport*) getParentComponent();
}

ViewportComponent::~ViewportComponent()
{
   	removeStimuliButtons();
}

//==============================================================================
void ViewportComponent::paint (Graphics& g)
{
    g.fillAll (Colours::white);
}

//==============================================================================
void ViewportComponent::resized()
{
    if (testLauncher == NULL)
		return;

	int tmpIndex;
	for (int i=0; i < trialCount; i++)
	{	
		tmpIndex = i*(stimCount+1);
		stimuliButtons[tmpIndex]->setBounds (REFBUTTON_X, REFBUTTON_Y + i*ROW_SPACING, REFBUTTON_W, REFBUTTON_H);

		for (int ind=1; ind <= stimCount; ind++)
		{
					
			tmpIndex = ind + i*(stimCount+1);
			stimuliButtons[tmpIndex]->setBounds (BUTTON_X + ind*BUTTON_SPACING, BUTTON_Y + i*ROW_SPACING, BUTTON_W, BUTTON_H);
		}
	}
}

void ViewportComponent::buttonClicked (Button* buttonThatWasClicked)
{
	selectedStimulusIndex = stimuliButtons.indexOf( (TextButton*) buttonThatWasClicked );
	selectStimulus( selectedStimulusIndex );
	sendChangeMessage();
}

void  ViewportComponent::addStimuliButtons()
{
	if (testLauncher == NULL)
	{
		return;
	}
	
	removeStimuliButtons();

	stimCount = testLauncher->getStimCount();
	trialCount = testLauncher->getTrialsCount();

	juce_wchar		letter;
	String			letterStr;
	String			tmpTestName;
	int				tmpIndex;

	for (int i=0; i < trialCount; i++)
	{	
		/* create reference buttons */
		tmpTestName = testLauncher->getTest(i)->testName;
		tmpIndex = i*(stimCount+1);
		
		stimuliButtons.add( new TextButton ( tmpTestName ) );
		stimButtonColours.add( referenceButtonColour );
		addAndMakeVisible ( stimuliButtons[tmpIndex] );

		stimuliButtons[tmpIndex]->setTooltip ( REF_TOOLTIP_TEXT + String(i));
		stimuliButtons[tmpIndex]->setButtonText ( tmpTestName );
		stimuliButtons[tmpIndex]->setColour( TextButton::buttonColourId, referenceButtonColour );	
		stimuliButtons[tmpIndex]->addListener (this);
		
		for (int ind=1; ind <= stimCount; ind++)
		{
			letter = 'A'+ ind - 1;
			letterStr = String::charToString(letter);
			
			tmpIndex = ind + i*(stimCount+1);

			stimuliButtons.add( new TextButton ( L"button" + letterStr ) );
			stimButtonColours.add( Colours::findColourForName( String(stimuliButtonColours[ind-1]).trimEnd(), Colours::antiquewhite ) );
			addAndMakeVisible ( stimuliButtons[tmpIndex] );
			stimuliButtons[tmpIndex]->setTooltip ( BUTTON_TOOLTIP_TEXT1 + letterStr + BUTTON_TOOLTIP_TEXT2 + String(i) );
			stimuliButtons[tmpIndex]->setButtonText ( letterStr );
			stimuliButtons[tmpIndex]->setColour( TextButton::buttonColourId, stimButtonColours[tmpIndex] );	
			stimuliButtons[tmpIndex]->addListener (this);
		}
	}

	int yMax = BUTTON_Y + (trialCount)*ROW_SPACING;

	setSize( getParentComponent()->getWidth() , yMax );
}

void  ViewportComponent::removeStimuliButtons()
{
	TextButton* tmpTextButton;
	for (int i=0; i<stimuliButtons.size(); i++)
	{
		deleteAndZero ( tmpTextButton = stimuliButtons[i] );
	}

	stimuliButtons.clear();
	stimButtonColours.clear();
}

void ViewportComponent::selectStimulus(int ind)
{
	
	if ((ind > -1) && (ind < stimuliButtons.size() ))
	{
		// deactivate all
		for (int i=0; i<stimuliButtons.size(); i++)
		{
			stimuliButtons[i]->setColour(TextButton::buttonColourId, stimButtonColours[i]);
		}

		// activate selected stimuli 
		stimuliButtons[ind]->setColour(TextButton::buttonColourId, activeButtonColour);	
	}
	// repaint the component
	repaint();
}

