#pragma once
#include <Render/UI/UIUnit.h>
#include <Render/UI/UIImage.h>

enum EBackgroundType : unsigned char
{
	BTNONE = 0,
	BTImage,
	BTColor
};

class QFAUIParentMultipleUnit;
class QFAUIParentOneUnit;
class QFAUIUnit;
class QFAWindow;
class QFAUIEvent;
class QFAUIScroll;
class QFAVKPipeline;
class QFAUIParent : public QFAUIUnit
{
	friend QFAUIParentMultipleUnit;
	friend QFAUIParentOneUnit;
	friend QFAUIUnit;
	friend QFAWindow;
	friend QFAUIEvent;
	friend QFAUIScroll;


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


	// child call if his slot change
	virtual void MySlotChange(QFAUIUnit* unit) = 0;
	/*

	unit be remove from this parent and unit not be Notify about
		use if unit be add to other parent or destroyed

*/
	virtual void RemoveUnitWithoutNotify(QFAUIUnit* unit) = 0;

	/*
		call in SetSizeParent	
	*/
	virtual void ChangeSize(unsigned int w, unsigned int h)  = 0;
	/*
		call in SetPositionParent
	*/
	virtual void ChangePosition(int x, int y) = 0;

	EOverflow Overflow = EOverflow::Visible;

	/*	
		parent call if need heve new InnerHeight
		write new valude in this->InnerHeight and return it
	*/
	virtual float UpdateInnerHeight() = 0;


	EParentType ParentType = EParentType::NONEChild;


	void SetSizeParent(unsigned int w, unsigned int h) final;
	void SetPositionParent(int x, int y) final;
public:
	QFAUIParent();
	~QFAUIParent();

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
	void SetBackgroundColor(FVector4D color);

	inline EParentType GetParentType()
	{
		return ParentType;
	}
protected:
	EBackgroundType BackgroundType = EBackgroundType::BTNONE;
	QFAUIImage BackgroundImage = QFAUIImage(nullptr);
	QFAImage* Image = nullptr;;
	FVector4D BackgroundColor = FVector4D(0);

	void RenderBackground(VkCommandBuffer comandebuffer);

	inline QFAVKPipeline* GetBackgroundPipeline()
	{
		return BackgroundImage.GetPipeline();
	}
};
