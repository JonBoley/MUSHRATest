#ifndef SUBJECTS_TABLE_LISTBOX
#define SUBJECTS_TABLE_LISTBOX

#include "../JuceLibraryCode/JuceHeader.h"

class SubjectTableListBox    :  public Component,
								public TableListBoxModel,
								public TextEditor::Listener
{
public:
	
	SubjectTableListBox(TableListBox* tListBox);
	
	SubjectTableListBox(TableListBox* tListBox, String xmlFileName);
	
	~SubjectTableListBox();

	
	void	textEditorTextChanged (TextEditor &editor);
	void	textEditorFocusLost	(TextEditor &editor); 	

	int 	getNumRows() { return rowsCount; };
	
	void 	paintRowBackground (Graphics &g, int rowNumber, int width, int height, bool rowIsSelected);
	
	void 	paintCell (Graphics &g, int rowNumber, int columnId, int width, int height, bool rowIsSelected);

	Component* refreshComponentForCell (int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate);
	
	void 	cellClicked (int rowNumber, int columnId, const MouseEvent &e);
	
	void 	cellDoubleClicked (int rowNumber, int columnId, const MouseEvent &e);
	
	void 	backgroundClicked ();
	
	void 	sortOrderChanged (int newSortColumnId, bool isForwards);

	int 	getColumnAutoSizeWidth (int columnId);

	const String 	getCellTooltip (int rowNumber, int columnId);

	void 	selectedRowsChanged (int lastRowSelected);

	void 	deleteKeyPressed (int lastRowSelected);

	void 	returnKeyPressed (int lastRowSelected);

	void 	listWasScrolled ();

	const var 	getDragSourceDescription (const SparseSet< int > &currentlySelectedRows);
	
	bool readDataFromFile(String xmlFileName);

	XmlElement* getColumns() { return columnList; }
	int			getNumColumns() {return colCount; }
	XmlElement*	getTableData() { return tableData; }

	TableListBox*	getParent() { return parent; }

	void	addSubject();
	void	deleteSubject();

private:
	TableListBox*	parent;
	
	int		rowsCount;
	int		colCount;
	int		lastSelected;
	Font	font;


    ScopedPointer<XmlElement> tableData;
	ScopedPointer<XmlElement> columnList; 

	String getAttributeNameForColumnId(int col);

	String getStringAttributeForCell(int row, int col) 
	{
		XmlElement* tmp = tableData->getChildElement(row);
		if (tmp)
		{
			return tmp->getStringAttribute( getAttributeNameForColumnId(col) );
		}
		else
		{
			return String::empty;
		}
	}

	bool setStringAttributeForCell(int row, int col, String newStr)
	{
		XmlElement* tmp = tableData->getChildElement(row);
		if (tmp)
		{
			tmp->setAttribute( getAttributeNameForColumnId(col), newStr );
			return true;
		}
		else
		{
			return false;
		}	
	}


	bool findCell(TextEditor &t, int &row, int &col)
	{
		row = -1;
		col = -1;

		for (int r = 0; r < rowsCount; ++r)
			for (int c = 1; c <= colCount; ++c)
			{
				if (t.getName() == L"textEditor_" + String(r) + L"_" + String(c) )
				{
					row = r;
					col = c;
					return true;
				}
			}
		
		return false;
	}


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SubjectTableListBox);
};




#endif
