#pragma once
#include <Render/UI/UIParentOneUnit.h>
#include <vector>

class QFAUIEvent;
class QFAUIScroll : public QFAUIParentOneUnit
{
	friend QFAUIEvent;
public:
	QFAUIScroll();
	~QFAUIScroll();

	/*
		Overflow always Hidden
	*/
	inline void SetOverflow(EOverflow over) override {}
private:
	void ChangedUnit(QFAUIUnit* unit) override;
	
	
	const float ScrollTime = 0.2f; 
	float ScrollTimeToEnd = 0.0f;
	float ScrollValue = 0.0f; // 
	float ScrollValueToEnd = 0.0f;
	float ScrollSpeed = 40.0f; // 40
	const float Ratio = 1 / ScrollTime;
	/*	
		value by which child be shifted relative to top this unit
	*/
	float ScrollValueFinal = 0.0f; 

	void ProcessScroll(float delta, float addValue = 0.0f);


	void MySlotChange(QFAUIUnit* unit) override;

	void SetSizeParent(unsigned int w, unsigned int h) override;
	void SetPositionParent(unsigned int x, unsigned int y) override;



	void AddScrollValueInside(float value);
	void ChangeScrollValueInside(float value);

	static std::vector<QFAUIScroll*> Scrolls;
	
	static void NewFrame(QFAUIScroll* scrollUndeCursor, float delta, float addValue = 0.0f);
	float UpdateInnerHeight() override;
};