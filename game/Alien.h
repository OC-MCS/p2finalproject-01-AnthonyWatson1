#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;
#include "HitBox.h"
#include "Weapons.h"
#include "ScreenMgr.h"
#include <map>
#include <list>

class AttackingActor
{
protected:
	float ProjectileSpeed;
	Texture*ProjectileTexture;
	map<int, Projectile*>* ActiveProjectiles;

public:
	AttackingActor(float projectileSpeed, Texture* projectileTexture, std::map<int, Projectile*>* activeProjectiles);

	//function should be overriden by the bomb and rocket classes
	virtual void FireProjectile() = 0;

	~AttackingActor()
	{

	}
};


AttackingActor::AttackingActor(float projectileSpeed,Texture* projectileTexture, map<int, Projectile*>* activeProjectiles)
{
	ProjectileSpeed = projectileSpeed;
	ProjectileTexture = projectileTexture;
	ActiveProjectiles = activeProjectiles;
}

int ScreenSizeX;
int ScreenSizeY;
int ProjectileId;
bool GameOver;

static const float minFrameTime = 0.03f;
static float timePassedSinceLastBombDropAttempt = 0;

class Enemy : public AttackingActor, virtual public MovingActor
{

public:
	Enemy(int id, float speed, float bombSpeed, pair<float, float> position,Texture* enemyTexture, Texture* bombTexture, std::map<int, Projectile*>* activeProjectiles);
	void Move(float delta);

	virtual void FireProjectile();
	virtual void Process(float delta);
	virtual void OnCollisionEnter();

	~Enemy();
};
Enemy::Enemy(int id, float speed, float bombSpeed, std::pair<float, float> position,Texture* enemyTexture, Texture* bombTexture, std::map<int, Projectile*>* activeProjectiles) :
	AttackingActor(bombSpeed, bombTexture, activeProjectiles), MovingActor(id, speed, std::pair<int, int>(1, 0), position, enemyTexture)
{
}
//delta parameter used to ensure consistent behaviour regardless of frame rate
void Enemy::Move(float delta)
{
	Position.first += delta * Speed * Direction.first;
	Position.second += ActorTexture->Height() * Direction.second;
}

//this function will handle the border checking, collision detection, direction change, bomb dropping
void Enemy::Process(float delta)
{
	//border checking
	//only destroy enemy is they are below the screen,
	//if they are to the edge then they will go down a step then continue in the
	//opposite direction
	if (Position.second > ScreenSizeY)
	{
		Alive = false;
		return;
	}

	//an enemy reached the bottom of the screen, so trigger the game over state
	if (Position.second > ScreenSizeY - ActorTexture->Height())
		GameOver = true;

	//only attempt to drop bombs max 30 times per second
	//this is done so that users with a higher fps do not
	//have more bombs dropped on them on average
	if (timePassedSinceLastBombDropAttempt > minFrameTime) {
		//from 1 to 1000
		int chanceOfDroppingBomb = rand() % 1000 + 1;

		//0.2% chance of dropping bomb
		if (chanceOfDroppingBomb >= 998) {
			FireProjectile();
		}

		timePassedSinceLastBombDropAttempt = 0;
	}

	timePassedSinceLastBombDropAttempt += delta;
}

//enemy dies when it's hit by a rocket
void Enemy::OnCollisionEnter()
{
	Alive = false;
}

void Enemy::FireProjectile() {
	(*ActiveProjectiles)[ProjectileId] = new Bomb(ProjectileId, ProjectileSpeed, std::pair<int, int>(Position.first, Position.second + ActorTexture->Height()), ProjectileTexture);
	ProjectileId++;
}

Enemy::~Enemy()
{
	std::cout << "Destructor for enemy: " << Id << " called \n";
}