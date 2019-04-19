#pragma once
#include <SFML/Graphics.hpp>
#include "HitBox.h"
#include "ScreenUI.h"
using namespace sf;

const int ScreenSizeX = 600;
const int ScreenSizeY = 800;

//Base Class for all the projectiles
class Projectile : public MovingActor
{
public:
	Projectile(int id, float speed, pair<int, int> direction, pair<float, float> startPosInPixels, Texture* texture)
	{

	}
	virtual void OnCollisionEnter();
	virtual void Process(float delta, Grid* grid) = 0;

	~Projectile()
	{

	}
};
//What the alien drops
class Bomb : public Projectile
{
public:
	Bomb(int id, float speed, pair<float, float> startPosInPixels,Texture* texture);

	virtual void Process(float delta, Grid* grid);

	~Bomb();
};
//What the player shoots
class Rocket : public Projectile
{
public:
	Rocket(int id, float speed, pair<float, float> startPosInPixels, Texture* texture);
	virtual void Process(float delta, Grid* grid);
	~Rocket();
};

//Projectile functions
void Projectile::OnCollisionEnter()
{
	Alive = false;
}


//The Bombs functions

Bomb::Bomb(int id, float speed, pair<float, float> startPos, Texture* texture) : Projectile(id, speed, pair<int, int>(0, 1), startPos, texture)
{
}
//this function will handle the border checking, and collision detection
void Bomb::Process(float delta, Grid* grid)
{
	//border checking
	//only destroy enemy is they are below the screen,
	//if they are to the edge then they will go down a step then continue in the
	//opposite direction
	if (Position.second > ScreenSizeY)
	{
		Alive = false;
	}

	list<GridEntry*> relevantEntries = grid->GetRelevantEntriesInGrid(this);

	for each (auto entry in relevantEntries)
	{
		//if bomb has been destroyed, finish processing
		if (Alive == false)
			return;

		//if this is not null, then it means the player is contained in this grid unit
		if (entry->PlayerActor != nullptr)
		{
			if (this->IsCollidingWith(entry->PlayerActor)) {
				//killing self
				Alive = false;

				//notifying player of collision
				entry->PlayerActor->OnCollisionEnter();
			}
		}
	}
}

Bomb::~Bomb()
{
}
//The Rocket Functions
//this function will handle the border checking, and collision detection
void Rocket::Process(float delta, Grid* grid)
{
	//border checking
	//only destroy enemy is they are below the screen,
	//if they are to the edge then they will go down a step then continue in the
	//opposite direction
	if (Position.second > ScreenSizeY)
	{
		Alive = false;
	}

	list<GridEntry*> relevantEntries = grid->GetRelevantEntriesInGrid(this);

	for each (auto entry in relevantEntries)
	{
		for each (auto enemy in entry->Enemies)
		{
			//if rocket has been destroyed, finish processing
			if (Alive == false)
				return;

			//only check enemy if they are alive
			if (enemy->IsAlive()) {
				if (this->IsCollidingWith(enemy)) {
					//killing self
					Alive = false;

					//notifying enemy of collision
					enemy->OnCollisionEnter();
				}
			}
		}
	}
}

Rocket::~Rocket()
{
}
