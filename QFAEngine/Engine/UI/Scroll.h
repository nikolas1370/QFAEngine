#pragma once
#include <UI/UIParentOneUnit.h>
#include <vector>

class QFAUIEvent;
class QFAEXPORT QFAUIScroll : public QFAUIParentOneUnit
{
	friend QFAUIEvent;
public:
	enum EScrollTYpe
	{
		STNot,
		STVertical,
		STHorizon,
		STBoth
	};

private:	
	const float ScrollTime = 0.2f;
	const double Ratio = 1 / ScrollTime;
	static std::vector<QFAUIScroll*> Scrolls;

	float ScrollSpeed = 40.0f; // 40

	EScrollTYpe ScrollType = STVertical;

	double ScrollTimeToEndVertical = 0.0f;
	float ScrollValueVertical = 0.0f; // 
	float ScrollValueToEndVertical = 0.0f;
	/*
		value by which child be shifted relative to top this unit
	*/
	float ScrollValueFinalVertical = 0.0f; // -value
	/*----*/
	double ScrollTimeToEndHorizon = 0.0f;
	float ScrollValueHorizon = 0.0f; // 
	float ScrollValueToEndHorizon = 0.0f;
	float ScrollValueFinalHorizon = 0.0f;

private:
	static void NewFrame(QFAUIScroll* scrollUndeCursor, double delta, float addValue = 0.0f, bool shiftDown = false);
	void ChangedUnit(QFAUIUnit* unit) override;

	void ProcessScroll(double delta, float addValue = 0.0f, bool shiftDown = false);

	//only onside usage	
	void ProcessScrollVerticalInside(double delta, float addValue);
	//only onside usage	
	void ProcessScrollHorizonInside(double delta, float addValue);
 
	void MySlotChange(QFAUIUnit* unit) override;

	void AddScrollValueVerticalInside(float value);
	void ChangeScrollValueVerticalInside(float value);

	void AddScrollValueHorizonInside(float value);
	void ChangeScrollValueHorizonInside(float value);



	float UpdateInnerHeight() override;
	float UpdateInnerWidth() override;	
protected:
	void WidthChanged(int oldValue = 0) override;
	void HeightChanged(int oldValue = 0) override;
	void TopChanged(int oldValue = 0) override;
	void LeftChanged(int oldValue = 0) override;

public:
	QFAUIScroll();
	~QFAUIScroll();



	

	/*
		Overflow always Hidden
	*/
	inline void SetOverflow(EOverflow over) override {}

	inline void SetScrollType(EScrollTYpe scrollType)
	{
		ScrollType = scrollType;
		ProcessScroll(0.0, 0.0f);
	}
};