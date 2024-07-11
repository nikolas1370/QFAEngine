#pragma once
#include <EngineStuff/Window/EngineViewport.h>


class QFAEXPORT QFAViewport : public QFAEngineViewport
{
public:
	QFAViewport();
	~QFAViewport();
	FVector2D GetPosition() final;
private:

};

