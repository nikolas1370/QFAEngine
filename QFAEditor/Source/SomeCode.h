#pragma once
#include <Window/Viewport.h>
#include <Window/Window.h>

#include <Object/Actor/Camera/CameraActor.h>
#include <Input/Input.h>
#include <UI/TextLocalization.h>
#include <Object/Class.h>

class SomeCode : public QActor
{
    QFACLASS(SomeCode)
    
public:
    SomeCode();
    ~SomeCode();
    int  l;
}; 




class QFAEngineWindow;
class ACameraEditor : public ACameraActor
{
    QFACLASS(ACameraEditor)
    FVector InputAxis = 0.0f;
    FVector2D MouseAxis = 0.0f;
    bool MosePress = false;
    QFAInput Input;
    float Speed = 1000;

    LOCALTEXT(Hedgehog, "Hedgehog")
protected:
    void Tick(float delta) override;
public:
    ACameraEditor();
    ~ACameraEditor();

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