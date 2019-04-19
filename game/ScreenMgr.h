#pragma once
#include <SFML/Graphics.hpp>
#include <list>
#include <iostream>
using namespace sf;


//Screen Mgr will handle everything displayed on
//the screen
//Tell whether or not the player won and have
//Losing and levels
class MovingActor
{
protected:
	int Id;
	float Speed;
	pair<int, int> Direction;
	pair<float, float> Position;
	list<std::pair<int, int>> RelevantEntriesInGrid;
	Texture* ActorTexture;
	bool Alive;

public:
	MovingActor(int id, float speed, pair<int, int> direction, pair<float, float> position, Texture* texture);
	virtual void Move(float delta);
	virtual void OnCollisionEnter() = 0;

	bool IsCollidingWith(MovingActor* actor);

	int GetId();
	float GetSpeed();
	bool IsAlive();
	pair<float, float> GetPosition();
	pair<int, int> GetDirection();
	Texture* GetTexture();

	void SetDirection(pair<int, int> newDir);
	void SetSpeed(float newSpeed);

	~MovingActor()
	{

	}
};
MovingActor::MovingActor(int id, float speed, pair<int, int> direction, pair<float, float> position, Texture* texture)
{
	Id = id;
	Speed = speed;
	Direction = direction;
	Position = position;
	ActorTexture = texture;
	Alive = true;
}

//this function can be overridden 
void MovingActor::Move(float delta)
{
	Position.first += delta * Speed * Direction.first;
	Position.second += delta * Speed * Direction.second;
}

bool MovingActor::IsCollidingWith(MovingActor * actor)
{
	list<pair<float, float>> myCorners{ pair<float,float>(Position.first, Position.second),
	pair<float,float>(Position.first + ActorTexture->Width(), Position.second),
	pair<float, float>(Position.first, Position.second + ActorTexture->Height()),
	pair<float, float>(Position.first + ActorTexture->Width(), Position.second + ActorTexture->Height()) };

	pair<float, float> otherActorPos = actor->GetPosition();
	Texture* otherActorTex = actor->GetTexture();

	for each (auto myCorner in myCorners)
	{
		//x coords are within range
		if (myCorner.first >= otherActorPos.first && myCorner.first <= otherActorPos.first + otherActorTex->Width())
		{
			//y coords are within range
			if (myCorner.second >= otherActorPos.second && myCorner.second <= otherActorPos.second + otherActorTex->Height())
				return true;
		}
	}

	return false;
}

int MovingActor::GetId()
{
	return Id;
}

float MovingActor::GetSpeed()
{
	return Speed;
}

bool MovingActor::IsAlive()
{
	return Alive;
}

pair<float, float> MovingActor::GetPosition()
{
	return Position;
}

pair<int, int> MovingActor::GetDirection()
{
	return Direction;
}

Texture * MovingActor::GetTexture()
{
	return ActorTexture;
}

void MovingActor::SetDirection(std::pair<int, int> newDir)
{
	Direction = newDir;
}

void MovingActor::SetSpeed(float newSpeed)
{
	Speed = newSpeed;
}
