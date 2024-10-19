#pragma once
//#include "../../QFAEngine/Engine/Object/Object.h"

#include <Window/Viewport.h>
#include <Window/Window.h>

#include <Object/Actor/Camera/CameraActor.h>
#include <Input/Input.h>

class SomeCode : public QActor//QObject
{
    QFAClassIn(SomeCode)
private:
    
public:
    SomeCode();
    ~SomeCode();
    int  l;
}; 




class QFAEngineWindow;
class ACameraEditor : public ACameraActor
{
    QFAClassIn(ACameraEditor);
    FVector InputAxis = 0.0f;
    FVector2D MouseAxis = 0.0f;
    bool MosePress = false;
    QFAInput Input;
    float Speed = 1000;
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