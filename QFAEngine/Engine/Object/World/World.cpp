#include "World.h"
#include <Object/Actor/Actor.h>

QWorld* QWorld::CurentWord;

void QWorld::ForgetActor(QActor* actor)
{
	for (int i = 0; i < ActorCount; i++)
	{
		if (Actors[i] == actor)
		{
			ActorCount--;
			Actors[i] = Actors[ActorCount];
			break;
		}
	}
}

QWorld::QWorld()
{
	Actors = new QActor*[ActorListLenght];// ActorListLenght
}

QWorld::~QWorld()
{
	for (size_t i = 0; i < ActorCount; i++)
		Actors[i]->Destroy();

	delete[] Actors;
}

void QWorld::AddActor(QActor* actor)
{
	if (!actor)
		return;

	if (ActorCount >= ActorListLenght)
	{
		QActor** tem = Actors;
		ActorListLenght *= 2;
		Actors = new QActor*[ActorListLenght];

		for (int i = 0; i < ActorCount; i++)
			Actors[i] = tem[i];
		
		delete[] tem;
	}

	Actors[ActorCount] = actor;
	ActorCount++;
}
