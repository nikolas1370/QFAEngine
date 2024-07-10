#pragma once
#include "Grid.h"
#include "Scroll.h"
#include "UIList.h"
#include "UIParentHiddenChild.h"
#include <Tools/Color.h>

class QFAUIEvent;
class QFAEXPORT QFAUISelectUnit abstract : public QFAParentHiddenChild
{
	friend QFAUIEvent;

	QFAUIScroll Scroll;
	struct SelectUnitEvent
	{
		std::function<void(QFAUIParent* unit)> InFocus;
		std::function<void()> OutFocus;
		std::function<void(QFAUIParent* unitUnderCursore)> LeftMouseDown;
		std::function<void(QFAUIParent* unitUnderCursore)> DobleClick; 
	};

	static std::vector<QFAUISelectUnit*> SelectUnitList;

private:
	QFAUIParentMultipleUnit* SelectUnitChild; // it's grid or list

	QFAUIParent* FocusUnit = nullptr;// can't be equal to SelectedUnit
	QFAUIParent* SelectedUnit = nullptr;
	bool SelectedUnitFocus = false;

	QFAUIParent* LastClickUnit = nullptr;
	size_t DobleClickTime = 400 * 10000;// convert from ms to 1/10 microseconds	
	size_t LastClickTime = 0;
public:
	QFAColor FocusColor = QFAColorF(1.0f, 1.0f, 1.0f, 0.05f);
	QFAColor SelectColor = QFAColorF(0.5f, 0.5f, 0.5f, 1.0f);
	QFAColor SelectLostFocusColor = QFAColorF(0.2f, 0.2f, 0.2f, 1.0f);

	SelectUnitEvent SelectEvent;

private:
	// call in QFAUIEvent need for track focus
	static void WindowLeftMouseDown(QFAEngineWindow* window, QFAUIUnit* unitUnderFocus);
	static void InFocus(QFAUIUnit* unit, void* _this);
	static void OutFocus(void* _this);
	static void LeftMouseDown(QFAUIUnit* unit, void* _this);


protected:

	void SetScrollChild(QFAUIParentMultipleUnit* child);

	void MySlotChange(QFAUIUnit* unit) final {}
	void ChangeSize(unsigned int w, unsigned int h) final;

	void ChangePosition(int x, int y) final;

	float UpdateInnerHeight() final;
	float UpdateInnerWidth() final;

public:

	QFAUISelectUnit();

	~QFAUISelectUnit();
	

	inline void AddUnit(QFAUIParent* unit)
	{
		SelectUnitChild->AddUnit(unit);
	}

	inline void RemoveUnit(QFAUIParent* unit)
	{
		SelectUnitChild->removeUnit(unit);
	}

	inline void RemoveAllUnit()
	{
		SelectUnitChild->removeAllUnit();
	}
	
	inline void SetScrollType(QFAUIScroll::EScrollTYpe type)
	{
		Scroll.SetScrollType(type);
	}

	inline void SetDobleClickTime(size_t time)
	{
		DobleClickTime = time * 10000;
	}
	
	inline QFAUIParent* GetSelectedUnit()
	{
		return SelectedUnit;
	}

	inline bool GetFocus()
	{
		return SelectedUnit ? SelectedUnitFocus : false;
	}

	void SetSelectUnit(QFAUIParent* unit);
	void SetFocus(bool focus)
	{
		SelectedUnitFocus = focus;
	}


};

class QFAUISelectGrid : public QFAUISelectUnit
{
	QFAUIGrid Grid;
public:
	QFAUISelectGrid()
	{
		SetScrollChild(&Grid);
	}

	inline void SetColumnCount(unsigned int columnCount)
	{
		Grid.SetColumnCount(columnCount);
	}

	inline void SetOffsets(unsigned int columnOffset, unsigned int rowOffset)
	{
		Grid.SetOffsets(columnOffset, rowOffset);
	}

	inline void SetUnitWidth(unsigned int unitWidth)
	{
		Grid.SetUnitWidth(unitWidth);
	}

	inline void SetMin(unsigned int min)
	{
		Grid.SetMin(min);
	}

	inline void SetRation(float ratio)
	{
		Grid.SetRation(ratio);
	}

	inline void SetPositionType(QFAUIGrid::UnitPositionType type)
	{
		Grid.SetPositionType(type);		
	}
private:

};

class QFAUISelectList : public QFAUISelectUnit
{
	QFAUIList List;
public:
	QFAUISelectList()
	{
		SetScrollChild(&List);
	}

	inline void SetListType(QFAUIList::EListType listType)
	{
		List.SetListType(listType);
	}
	inline QFAUIList::EListType GetListType()
	{
		return List.GetListType();
	}

	inline void SetUnitHeight(unsigned int h)
	{
		List.SetUnitHeight(h);
	}

	inline void SetUnitWidth(unsigned int w)
	{
		List.SetUnitWidth(w);
	}

	inline void SetStretchLastUnit(bool enable)
	{
		List.SetStretchLastUnit(enable);
	}
private:

};



/*
* in void QFAEditorMainWindow::PrepareCallback()
*
	/*

					remove in SelectUnit

	
		*/



