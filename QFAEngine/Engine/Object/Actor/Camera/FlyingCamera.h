#pragma once
#include <Core/Window/Viewport.h>
#include <Core/Window/Window.h>
#include <Object/Actor/Camera/CameraActor.h>
#include <Input/Input.h>

class QFAEngineWindow;
class QFAEXPORT AFlyingCamera : public ACameraActor
{
    QFAEngineClassIn(AFlyingCamera)
    FVector InputAxis = 0.0f;
    FVector2D MouseAxis = 0.0f;
    bool MosePress = false;
    QFAInput Input;
    float Speed = 1000;
protected:
    void Tick(float delta) override;
public:
    AFlyingCamera();
    ~AFlyingCamera();

    void SetWindowForInput(QFAEngineWindow* window);
    inline float GetSpeed()
    {
        return Speed;
    }
    inline void SetSpeed(float speed)
    {
        Speed = speed;
    }

    inline void ActiveInput(bool activate)
    {
        Input.ActiveInput(activate);
    }
private:

};
