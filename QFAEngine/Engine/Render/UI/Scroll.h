#pragma once
#include <Render/UI/UIParentOneUnit.h>
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
private:
	void ChangedUnit(QFAUIUnit* unit) override;
	float ScrollSpeed = 40.0f; // 40
	const float ScrollTime = 0.2f;
	const double Ratio = 1 / ScrollTime;

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


	void ProcessScroll(double delta, float addValue = 0.0f, bool shiftDown = false);

	//only onside usage	
	void ProcessScrollVerticalInside(double delta, float addValue);
	//only onside usage	
	void ProcessScrollHorizonInside(double delta, float addValue);
 
	void MySlotChange(QFAUIUnit* unit) override;

	void ChangeSize(unsigned int w, unsigned int h) override;
	void ChangePosition(int x, int y) override;



	void AddScrollValueVerticalInside(float value);
	void ChangeScrollValueVerticalInside(float value);

	void AddScrollValueHorizonInside(float value);
	void ChangeScrollValueHorizonInside(float value);

	static std::vector<QFAUIScroll*> Scrolls;
	
	static void NewFrame(QFAUIScroll* scrollUndeCursor, double delta, float addValue = 0.0f, bool shiftDown = false);
	float UpdateInnerHeight() override;
	float UpdateInnerWidth() override;
	
};