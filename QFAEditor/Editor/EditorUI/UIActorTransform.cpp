#include "UIActorTransform.h"
#include <Render/UI/Canvas.h>
#include <Render/UI/TextInput.h>
#include <Render/UI/Background.h>
#include <Object/Actor/Actor.h>
#include <Tools/String.h>



QFAUIActorTransform* QFAUIActorTransform::ActorTransform;

QFAUIActorTransform::QFAUIActorTransform()
{	
	Canvas = new QFAUICanvas;
	
	struct SSlot
	{
		float x = 0.0f;
		float y = 0.0f;
	};
	std::array<SSlot, 9> SSlots;
	SSlots[0] = { 0.015f , 0.0f};
	SSlots[1] = { 0.345f, 0.0f};
	SSlots[2] = { 0.675f, 0.0f};
	SSlots[3] = { 0.015f , (float)InputHeight + 5 };
	SSlots[4] = { 0.345f, (float)InputHeight + 5 };
	SSlots[5] = { 0.675f, (float)InputHeight + 5 };
	SSlots[6] = { 0.015f , 60.0f};
	SSlots[7] = { 0.345f, 60.0f};
	SSlots[8] = { 0.675f, 60.0f};

	QFAUISlot::SCanvasSlot slot;
	slot.Width = 0.315f;
	slot.Height = (float)InputHeight;
	slot.HeightInPixel = true;
	slot.yInPixel = true;


	AddHiddenChild(Canvas);
	QFAUITextInput* input;
	QFAUIBackground* inputBackground;
	for (size_t i = 0; i < Inputs.size(); i++)
	{
		inputBackground = new QFAUIBackground;
		input = new QFAUITextInput(QFAUITextInput::ENumberType::Float);
		input->SetTextSize(20);
		input->SetTextColor(255);
		input->SetOutFocusFun(QFAUIActorTransform::InputOut);
		inputBackground->SetUnit(input);
		Canvas->AddUnit(inputBackground);
		
		Inputs[i] = input;
		InputBackgrounds[i] = inputBackground;
		InputBackgrounds[i]->SetBackgroundColor(QFAColor(20, 20, 20));

		slot.x = SSlots[i].x;
		slot.y = SSlots[i].y;
		inputBackground->SetSlot(&slot);
	}

	ActorTransform = this;
}

QFAUIActorTransform::~QFAUIActorTransform()
{
	for (size_t i = 0; i < Inputs.size(); i++)
	{
		delete Inputs[i];
		delete InputBackgrounds[i];
	}

	delete Canvas;
}

void QFAUIActorTransform::MySlotChange(QFAUIUnit* unit)
{

}

void QFAUIActorTransform::ChangeSize(unsigned int w, unsigned int h)
{
	Width = w;
	Height = h;
	SetChildSize(Canvas, w, h);
}

void QFAUIActorTransform::ChangePosition(int x, int y)
{
	Position_x = x;
	Position_y = y;
	SetChildPosition(Canvas, x, y);
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
					std::cout << "lopopid\n";
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
