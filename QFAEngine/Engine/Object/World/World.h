#pragma once
#include <Object/Object.h>
#include <Object/World/DirectionLight/DirectionLight.h>

class QActor;

class QWorld : public QObject
{
	/*- in futere be some else  */
public:

	inline static QWorld* GetCurentWorld()
	{
		return QWorld::CurentWord;
	}
	inline static void SetCurentWorld(QWorld* world)
	{
		QWorld::CurentWord = world;
	}

private:
	static QWorld* CurentWord;

	/*---*/


	friend QActor;
	QDirectionLight DirectionLight;

	

	// call only from QActor Destructor
	void ForgetActor(QActor* actor);
public:
	
	/*--in private */

	QActor** Actors;
	unsigned int ActorListLenght = 100;
	unsigned int ActorCount = 0;

	/*--*/


	QWorld();
	~QWorld();

	void AddActor(QActor* actor);
	
	inline QDirectionLight* GetDirectionDight()
	{
		return &DirectionLight;
	}
private:

};
