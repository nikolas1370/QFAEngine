#include "pch.h"
#include "FlyingCamera.h"

QFAEngineClassOut(AFlyingCamera)
AFlyingCamera::AFlyingCamera()
{
    // set up movement events
    QFAInputAxis3D ax = Input.CreateAxis3D("move", [this](FVector axis)
        {
            this->InputAxis = axis;
        });
    // all value be added and send in ax
    // if wanna delte key use ax.RemoveKey
    ax.AddKey(EKey::W, FVector(1, 0, 0), "forward");
    ax.AddKey(EKey::S, FVector(-1, 0, 0), "backward");
    ax.AddKey(EKey::A, FVector(0, -1, 0), "left");
    ax.AddKey(EKey::D, FVector(0, 1, 0), "right");
    ax.AddKey(EKey::SPACE, FVector(0, 0, 1), "up");
    ax.AddKey(EKey::LEFT_CONTROL, FVector(0, 0, -1), "down");

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

    Camera.Activate();

    QFAWindow* window = QFAWindow::GetWindow();
    QFAViewport* viewport = window->GetViewport(0);
    ActivateCamera(viewport); // attached camera to viewport

    SetTick(true);
}

AFlyingCamera::~AFlyingCamera()
{
    SeparateRootComponent();
}

void AFlyingCamera::SetWindowForInput(QFAEngineWindow* window)
{
    Input.ChangeWindow(window);
}

void AFlyingCamera::Tick(float delta)
{
    QSuper::Tick(delta);
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
