#include "UIParent.h"

void QFAUIParent::SetSizeParent(unsigned int w, unsigned int h)
{
	BackgroundImage.SetSizeParent(w, h);
	// background
	ChangeSize(w, h);
}

void QFAUIParent::SetPositionParent(int x, int y)
{
	BackgroundImage.SetPositionParent(x, y);
	ChangePosition(x, y);
}

QFAUIParent::QFAUIParent()
{
	CanBeParent = true;
	BackgroundImage.Parent = this;
}

QFAUIParent::~QFAUIParent()
{
	BackgroundImage.Parent = nullptr;
}

void QFAUIParent::SetBackgroundImage(QFAImage* image)
{
	Image = image;
	BackgroundImage.SetImage(image);
}



void QFAUIParent::RenderBackground(VkCommandBuffer comandebuffer)
{
	if (Parent->Type == QFAUIType::Scroll)
		if (Parent->Position_x != BackgroundImage.Position_x || Parent->Position_y != BackgroundImage.Position_y)
			BackgroundImage.SetPositionParent(Parent->Position_x, Parent->Position_y);

	BackgroundImage.SetZIndex(ZIndex);
	BackgroundImage.Render(comandebuffer);
}

void QFAUIParent::SetBackgroundType(EBackgroundType type)
{
	BackgroundType = type;
	switch (type)
	{
		case EBackgroundType::BTNONE:
			BackgroundImage.SetBackgroundColor(0);
			BackgroundImage.SetImage(nullptr);
			break;
		case EBackgroundType::BTImage:
			BackgroundImage.SetImage(Image);
			break;
		case EBackgroundType::BTColor:
			BackgroundImage.SetImage(nullptr);
			BackgroundImage.SetBackgroundColor(BackgroundColor);
			break;
		default:
			break;
	}
}

void QFAUIParent::SetChildPosition(QFAUIUnit* childUnit, int x, int y)
{
	if(childUnit->IsValid() && childUnit->Parent == this)
		childUnit->SetPositionParent(x, y);
}

void QFAUIParent::SetChildSize(QFAUIUnit* childUnit, int w, int h)
{
	if (childUnit->IsValid() && childUnit->Parent == this)
		childUnit->SetSizeParent(w, h);
}
