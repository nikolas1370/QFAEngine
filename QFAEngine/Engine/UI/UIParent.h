#pragma once
#include <UI/UIUnit.h>
#include <UI/UIImage.h>

enum EBackgroundType : unsigned char
{
	BTNONE = 0,
	BTImage,
	BTColor
};

class QFAUIParentMultipleUnit;
class QFAUIParentOneUnit;
class QFAUIUnit;
class QFAEngineWindow;
class QFAUIEvent;
class QFAUIScroll;
class QFAVKPipeline;
class QFAUIList;
class QFAText;
class QFAUIBackground;
class QFAUITextInput;

class QFAEXPORT QFAUIParent : public QFAUIUnit
{
	friend QFAUIParentMultipleUnit;
	friend QFAUIParentOneUnit;
	friend QFAUIUnit;
	friend QFAEngineWindow;
	friend QFAUIEvent;
	friend QFAUIScroll;
	friend QFAUIList;
	friend QFAText;
	friend QFAUIBackground;
	friend QFAUITextInput;

public:
	enum EOverflow : unsigned char
	{
		Visible = 0,
		Hidden = 1,
		HiddenHorizon = 2,
		HiddenVertical = 3
	};

	enum EParentType : unsigned char
	{
		NONEChild,
		MultipleChild,
		OneChild,
		HiddenChild
	};

protected:
	EBackgroundType BackgroundType = EBackgroundType::BTNONE;
	//QFAUIImage BackgroundImage = QFAUIImage(nullptr, true);
	QFAImage* Image = nullptr;;
	QFAColorF BackgroundColor;

	

	EOverflow Overflow = EOverflow::Visible;

	EParentType ParentType = EParentType::NONEChild;
	
	/*

	unit be remove from this parent and unit not be Notify about
		use if unit be add to other parent or destroyed

*/
	virtual void RemoveUnitWithoutNotify(QFAUIUnit* unit) = 0;
	// child call if his slot change
	virtual void MySlotChange(QFAUIUnit* unit) = 0;


	/*
		parent call if need heve new InnerHeight. 
		write new valude in this->InnerHeight and return it 		
	*/
	virtual float UpdateInnerHeight() = 0;
	virtual float UpdateInnerWidth() = 0;

	// child class call if change size
	void RecalculateBackgroundSize();	
	void RecalculateBackgroundPosition();

	/*
		child call if inner Height/Width change
	*/
	virtual void ChildInnerChange(QFAUIUnit* child) {};


	void RenderBackground(VkCommandBuffer comandebuffer);

	inline QFAVKPipeline* GetBackgroundPipeline()
	{
		return BackgroundImage.GetPipeline();
	}

	/*
	* 
	* use setwidth and etc
	* 
	// use only in child class with unit type QFAUIType::CustomUnit
	void SetChildPosition(QFAUIUnit* childUnit, int x, int y);
	// use only at child class with unit type QFAUIType::CustomUnit
	void SetChildSize(QFAUIUnit* childUnit, int w, int h);
	*/
public:
	QFAUIParent();
	~QFAUIParent();

	// reemove in priv
	QFAUIImage BackgroundImage = QFAUIImage(nullptr, true);

	inline EOverflow GetOverflow()
	{
		return Overflow;
	}

	virtual inline void SetOverflow(EOverflow over)
	{
		Overflow = over;
	}

	void SetBackgroundType(EBackgroundType type);
	inline EBackgroundType GetBackgroundType()
	{
		return BackgroundType;
	}

	void SetBackgroundImage(QFAImage* image);

	void SetBackgroundColor(QFAColor color)
	{
		BackgroundColor = color.GetColorF();
		BackgroundImage.SetBackgroundColor(BackgroundColor);
	}

	void SetBackgroundColor(QFAColorB color)
	{
		BackgroundColor = QFAColor(color).GetColorF();
		BackgroundImage.SetBackgroundColor(BackgroundColor);
	}

	void SetBackgroundColor(QFAColorF color)
	{
		BackgroundColor = color;
		BackgroundImage.SetBackgroundColor(BackgroundColor);
	}

	inline EParentType GetParentType()
	{
		return ParentType;
	}
};
