#include "SomeCode.h"
#include <iostream>



SomeCode::SomeCode()
{
    std::cout << "SomeCode constructor\n" ;
}

SomeCode::~SomeCode()
{
    std::cout << "~SomeCode destructor\n" ;
}

/*-------------*/


ACameraEditor::ACameraEditor()
{
    QFAInputAxis3D ax = Input.CreateAxis3D("move", [this](FVector axis)
        {
            this->InputAxis = axis;
        });

    Input.SetMouseMoveAxis([this](FVector2D axis)
        {
            this->MouseAxis = axis;
        });

    Input.AddKeyPress(EKey::MOUSE_LEFT, "mouse", [this](EKey::Key)
        {
            this->MosePress = true;
        });

    Input.AddKeyRelease(EKey::MOUSE_LEFT, "mouse", [this](EKey::Key)
        {
            this->MosePress = false;
        });

    SetTick(true);

    ax.AddKey(EKey::W, FVector(1, 0, 0), "forward");
    ax.AddKey(EKey::S, FVector(-1, 0, 0), "backward");
    ax.AddKey(EKey::A, FVector(0, -1, 0), "left");
    ax.AddKey(EKey::D, FVector(0, 1, 0), "right");
    ax.AddKey(EKey::SPACE, FVector(0, 0, 1), "up");
    ax.AddKey(EKey::LEFT_CONTROL, FVector(0, 0, -1), "down");
    Camera.Activate();
    Camera.SetLocalPosition(FVector(-100, 0, 0));
    Camera.SetRotation(0);


    ActivateCamera(QFAWindow::GetWindow()->GetViewport(0));
}

ACameraEditor::~ACameraEditor()
{
    SeparateRootComponent();
    std::cout << "~ACameraEditor " << "\n";
}

void ACameraEditor::SetWindowForInput(QFAEngineWindow* window)
{
    Input.ChangeWindow(window);
}


void ACameraEditor::Tick(float delta)
{
    if (!Camera.GetStatus())
        return;

    if (MosePress)
    {
        FVector rot = GetActorRotation();
        rot.Y += MouseAxis.Y * 0.1f * -1;
        rot.Z += MouseAxis.X * 0.1f;

        if (rot.Y > 90)
            rot.Y = 90;
        else if (rot.Y < -90)
            rot.Y = -90;

        SetActorRotation(rot);
        MouseAxis = FVector2D(0);
    }


    SetActorPosition((GetActorForwardVector() * InputAxis.X + GetActorRightVector() * InputAxis.Y + GetActorUpVector() * InputAxis.Z).Normalize() * delta * Speed + GetActorPosition());
}
