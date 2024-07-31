#include "pch.h"
#include "UIParent.h"

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
	RecalculateBackgroundSize();
	RecalculateBackgroundPosition();
}

void QFAUIParent::RecalculateBackgroundSize()
{
	BackgroundImage.ParentSetHeight = Height;
	BackgroundImage.ParentSetWidth = Width;
	BackgroundImage.SetWidth("100%", false);
	BackgroundImage.SetHeight("100%", false);
}

void QFAUIParent::RecalculateBackgroundPosition()
{
	BackgroundImage.ParentSetPosition_y = Position_y;
	BackgroundImage.ParentSetPosition_x = Position_x;
	BackgroundImage.SetTop(nullptr);
	BackgroundImage.SetLeft(nullptr);
}

void QFAUIParent::RenderBackground(VkCommandBuffer comandebuffer)
{
	if (Parent->Type == QFAUIType::Scroll)
		if (Parent->Position_x != BackgroundImage.Position_x || Parent->Position_y != BackgroundImage.Position_y)
		{
			BackgroundImage.ParentSetPosition_x = Position_x;
			BackgroundImage.ParentSetPosition_y = Position_y;
			BackgroundImage.SetTop(BackgroundImage.StrTop);
			BackgroundImage.SetLeft(BackgroundImage.StrLeft);
		}

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

