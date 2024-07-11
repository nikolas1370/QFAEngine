#include "pch.h"
#include "Viewport.h"
#include <EngineStuff/Window/ViewportHolder.h>

QFAViewport::QFAViewport()
{
}

QFAViewport::~QFAViewport()
{
}

FVector2D QFAViewport::GetPosition()
{	
#if QFA_EDITOR_ONLY
	if (Window->RegularWindow)
		return FVector2D(X, Y);
	else
		return FVector2D(X + Window->Holder->X, Y + Window->Holder->Y);
#else
	return FVector2D(X, Y);
#endif
}

