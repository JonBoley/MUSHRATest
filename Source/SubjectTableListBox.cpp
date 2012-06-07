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

#include "SubjectTableListBox.h"

static const unsigned char columnSpecs[] =
"    <COLUMNS>\r\n"
"        <COLUMN columnId=\"1\" name=\"Subject\" width=\"0.25\"/>\r\n"
"        <COLUMN columnId=\"2\" name=\"Stimuli folder\" width=\"0.75\"/>\r\n"
"    </COLUMNS>\r\n";



SubjectTableListBox::SubjectTableListBox(TableListBox* tListBox):	parent(tListBox),
							font (14.0f),
							rowsCount(0),
							colCount(2),
							lastSelected(-1)
{
};
	
SubjectTableListBox::SubjectTableListBox(TableListBox* tListBox, String xmlFileName):	parent(tListBox),
												font (14.0f),
												rowsCount(0),
												colCount(2),
												lastSelected(-1)
{
	readDataFromFile(xmlFileName);
};
	

SubjectTableListBox::~SubjectTableListBox() {};

	
void 	SubjectTableListBox::paintRowBackground (Graphics &, int , int , int , bool )
{
}
	
void 	SubjectTableListBox::paintCell (Graphics &, int , int , int , int , bool ) 
{
}

void 	SubjectTableListBox::cellClicked (int , int , const MouseEvent &) {};

void 	SubjectTableListBox::cellDoubleClicked (int , int , const MouseEvent &) {};

void 	SubjectTableListBox::backgroundClicked () {};

void 	SubjectTableListBox::sortOrderChanged (int , bool ) {};

int 	SubjectTableListBox::getColumnAutoSizeWidth (int columnId) 
{
	int w = parent->getHeader().getTotalWidth();
	XmlElement* tmp = columnList->getChildElement(columnId-1);
	double rel = w * tmp->getDoubleAttribute(L"width");
	return (int) rel;
};


const String 	SubjectTableListBox::getCellTooltip (int /*rowNumber*/, int columnId)
{
	switch (columnId)
	{
	case 0: return L"Subject Name"; break;
	case 1: return L"Stimuli Directory"; break;
	default: return String::empty;
	}
}

void 	SubjectTableListBox::selectedRowsChanged (int ) {};

void 	SubjectTableListBox::deleteKeyPressed (int ) {};

void 	SubjectTableListBox::returnKeyPressed (int ) {};

void 	SubjectTableListBox::listWasScrolled () {};

const var 	SubjectTableListBox::getDragSourceDescription (const SparseSet< int > &) {return var::null;};

Component* SubjectTableListBox::refreshComponentForCell (int rowNumber, int columnId, bool /*isRowSelected*/, Component* existingComponentToUpdate)
{
	if (columnId <= 2) 
    {
        TextEditor* textBox = (TextEditor*) existingComponentToUpdate;

		// If an existing component is being passed-in for updating, we'll re-use it, but
        // if not, we'll have to create one.
        if (textBox == NULL)
		{	
			textBox = new TextEditor(L"textEditor_" + String(rowNumber) + L"_" + String(columnId) );

			textBox->setBoundsInset (BorderSize<int> (2));
			textBox->setBorder( BorderSize<int> (0) );
			const uint8 colcomp = 0;
			Colour transp( colcomp ,colcomp ,colcomp ,colcomp);
			textBox->setColour( TextEditor::outlineColourId, transp);
			textBox->setColour( TextEditor::shadowColourId, transp );

			textBox->setText( getStringAttributeForCell(rowNumber, columnId)  );
			textBox->addListener(this);
			addAndMakeVisible(textBox);
			
		}
		else
		{
			textBox->setText( getStringAttributeForCell(rowNumber, columnId)  );
		}

		return textBox;
    }
    else
    {
        // for any other column, just return 0, as we'll be painting these columns directly.
        jassert (existingComponentToUpdate == 0);
        return 0;
    }
}
	
bool SubjectTableListBox::readDataFromFile(String xmlFileName)
{

	File tmpFile(xmlFileName);
	XmlDocument tempDocument (tmpFile); 

	tableData = tempDocument.getDocumentElement();
    if (tableData == NULL)
	{
		tableData = new XmlElement(L"DATA");
	}
	rowsCount = tableData->getNumChildElements();
	
	XmlDocument tempColDocument ( String( (const char*) columnSpecs) ); 		
    columnList = tempColDocument.getDocumentElement();
	if (columnList == NULL)
		return false;

	return true;
}

void	SubjectTableListBox::textEditorTextChanged (TextEditor &editor)
{
	int row = -1;
	int col = -1;

	if (findCell(editor, row, col))
	{
		setStringAttributeForCell(row, col, editor.getText());
	}
}	


void  SubjectTableListBox::textEditorFocusLost(TextEditor &editor)
{
	int row = -1;
	int col = -1;

	if (findCell(editor, row, col))
	{
		lastSelected = row;
	}
}


String SubjectTableListBox::getAttributeNameForColumnId(int col)
{
		switch (col)
		{
		case 1: return L"name"; break;
		case 2: return L"stimuliDirectory"; break;
		default: return String::empty;
		}
}

void SubjectTableListBox::addSubject()
{
	XmlElement* element = new XmlElement(L"subject");
	element->setAttribute( getAttributeNameForColumnId(1), String::empty );
	element->setAttribute( getAttributeNameForColumnId(2), String::empty );
	tableData->addChildElement(element);
	rowsCount = tableData->getNumChildElements();
	parent->updateContent();

	TextEditor* t = (TextEditor* )parent->getCellComponent( 1, rowsCount-1);
	if (t)
	{
		t->grabKeyboardFocus();
	}
}

void SubjectTableListBox::deleteSubject()
{
	if (lastSelected == -1)
		return;
	
	/* delete last selected element */
	XmlElement *victim = tableData->getChildElement(lastSelected);
	if (victim)
	{
		tableData->removeChildElement(victim, true);
		rowsCount = tableData->getNumChildElements();
		parent->updateContent();
		lastSelected = -1;
		return;
	}

}



