#include "epch.h"
#include "UIActorTransform.h"
#include <UI/Canvas.h>
#include <UI/TextInput.h>
#include <UI/Background.h>
#include <Object/Actor/Actor.h>
#include <Tools/String.h>
#include <EngineClassesInterface.h>

QFAUIActorTransform* QFAUIActorTransform::ActorTransform;

QFAUIActorTransform::QFAUIActorTransform()
{	
	Canvas = NewUI<QFAUICanvas>();
	
	struct SSlot
	{
		const char* x = "0.0";
		const char* y = "0.0";
	};
	std::array<SSlot, 9> SSlots;
	SSlots[0] = { "1.5%",   "0"};
	SSlots[1] = { "34.5%",  "0" };
	SSlots[2] = { "67.5%",  "0" };
	SSlots[3] = { "01.5%",  "30" }; // InputHeightChar + 5
	SSlots[4] = { "34.5%",  "30" }; // InputHeightChar + 5
	SSlots[5] = { "67.5%",  "30" }; // InputHeightChar + 5
	SSlots[6] = { "01.5%",  "60"};
	SSlots[7] = { "34.5%",  "60"};
	SSlots[8] = { "67.5%",  "60"};

	AddHiddenChild(Canvas);
	QFAUITextInput* input;
	for (size_t i = 0; i < Inputs.size(); i++)
	{
		input = NewUI<QFAUITextInput>(QFAUITextInput::ENumberType::Float);
		input->SetTextSize(20);
		input->SetTextColor(255);
		input->SetOutFocusFun(QFAUIActorTransform::InputOut);
		Canvas->AddUnit(input);
		
		Inputs[i] = input;
		Inputs[i]->SetBackgroundColor(QFAColor(20, 20, 20));

		input->SetWidth("31.5%");
		input->SetHeight(InputHeightChar);
		input->SetTop(SSlots[i].y);
		input->SetLeft(SSlots[i].x);
	}

	ActorTransform = this;
}

QFAUIActorTransform::~QFAUIActorTransform()
{
	for (size_t i = 0; i < Inputs.size(); i++)
		Inputs[i]->Destroy();

	Canvas->Destroy();
}

void QFAUIActorTransform::MySlotChange(QFAUIUnit* unit)
{

}


void QFAUIActorTransform::WidthChanged(int oldValue)
{
	((QFAEditorCanvas*)Canvas)->ParentSetWidth = Width;
	Canvas->SetWidth(((QFAEditorCanvas*)Canvas)->StrWidth, ((QFAEditorCanvas*)Canvas)->ParentSetWidthMinus);
}

void QFAUIActorTransform::HeightChanged(int oldValue)
{
	((QFAEditorCanvas*)Canvas)->ParentSetHeight = Height;
	Canvas->SetHeight(((QFAEditorCanvas*)Canvas)->StrHeight, ((QFAEditorCanvas*)Canvas)->ParentSetHeightMinus);
}

void QFAUIActorTransform::TopChanged(int oldValue)
{
	((QFAEditorCanvas*)Canvas)->ParentSetPosition_y = Position_y;
	Canvas->SetTop(((QFAEditorCanvas*)Canvas)->StrTop);
}

void QFAUIActorTransform::LeftChanged(int oldValue)
{
	((QFAEditorCanvas*)Canvas)->ParentSetPosition_x = Position_x;
	Canvas->SetLeft(((QFAEditorCanvas*)Canvas)->StrLeft);
}

float QFAUIActorTransform::UpdateInnerHeight()
{
	return InputHeight * 3;
}

float QFAUIActorTransform::UpdateInnerWidth()
{
	return Width;
}

void QFAUIActorTransform::InputOut(QFAUITextInput* input)
{
	for (size_t i = 0; i < ActorTransform->Inputs.size(); i++)
	{
		if(ActorTransform->Inputs[i] == input)
		{
			if (!ActorTransform->CurentSelectActor->IsValid())
				return;

			QActor* actor = ActorTransform->CurentSelectActor;
			FVector tem;
			switch (i)
			{
				case 0: 
					tem = actor->GetActorPosition();
					tem.X = QFAString::ToFloat(ActorTransform->Inputs[0]->GetValue());
					actor->SetActorPosition(tem);
					break;
				case 1: 
					tem = actor->GetActorPosition();
					tem.Y = QFAString::ToFloat(ActorTransform->Inputs[1]->GetValue());
					actor->SetActorPosition(tem);
					break;
				case 2: 
					tem = actor->GetActorPosition();
					tem.Z = QFAString::ToFloat(ActorTransform->Inputs[2]->GetValue());
					actor->SetActorPosition(tem);
					break;

				case 3: 
					tem = actor->GetActorRotation();
					tem.X = QFAString::ToFloat(ActorTransform->Inputs[3]->GetValue());
					actor->SetActorRotation(tem);
					break;
				case 4: 
					tem = actor->GetActorRotation();
					tem.Y = QFAString::ToFloat(ActorTransform->Inputs[4]->GetValue());
					actor->SetActorRotation(tem);
					break;
				case 5: 
					tem = actor->GetActorRotation();
					tem.Z = QFAString::ToFloat(ActorTransform->Inputs[5]->GetValue());
					actor->SetActorRotation(tem);
					break;

				case 6: 
					tem = actor->GetActorScale();
					tem.X = QFAString::ToFloat(ActorTransform->Inputs[6]->GetValue());
					actor->SetActorScale(tem);
					break;
				case 7: 
					tem = actor->GetActorScale();
					tem.Y = QFAString::ToFloat(ActorTransform->Inputs[7]->GetValue());
					actor->SetActorScale(tem);
					break;
				case 8: 
					tem = actor->GetActorScale();
					tem.Z = QFAString::ToFloat(ActorTransform->Inputs[8]->GetValue());
					actor->SetActorScale(tem);
					break;
			}

			return;
		}
	}
}

void QFAUIActorTransform::SelectActor(QActor* actor)
{	
	CurentSelectActor = actor;
	if (actor->IsValid())
	{
		FVector tem = actor->GetActorPosition();
		Inputs[0]->SetValue(tem.X);
		Inputs[1]->SetValue(tem.Y);
		Inputs[2]->SetValue(tem.Z);

		tem = actor->GetActorRotation();
		Inputs[3]->SetValue(tem.X);
		Inputs[4]->SetValue(tem.Y);
		Inputs[5]->SetValue(tem.Z);

		tem = actor->GetActorScale();	
		Inputs[6]->SetValue(tem.X);
		Inputs[7]->SetValue(tem.Y);
		Inputs[8]->SetValue(tem.Z);
	}
}
