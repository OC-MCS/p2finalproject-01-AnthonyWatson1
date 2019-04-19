#pragma once
#include <SFML/Graphics.hpp>
#include "HitBox.h"
#include "Weapons.h"
#include "Alien.h"
#include "ScreenMgr.h"
using namespace sf;

int ScreenSizeX = 800;
int ScreenSizeY = 600;
float timePassedSinceLastRocketFired = 0;



//The player object
class Player : virtual public AttackingActor, virtual public MovingActor
{
private:
	int Health;
	gfx::InputHandler* InputHandler;

public:
	Player(int health,float speed,float rocketSpeed,Texture* playerTexture,Texture* rocketTexture,gfx::InputHandler* inputHandler,map<int, Projectile*>* activeProjectiles);

	virtual void FireProjectile();
	void Process(float delta, Grid* grid);
	virtual void OnCollisionEnter();

	void Move(float delta);
	void TakeDamage(int damage);

	int GetHealth();
	~Player();
};
Player::Player(int health, float speed, float rocketSpeed, gfx::Texture * playerTexture, gfx::Texture * rocketTexture, gfx::InputHandler* inputHandler, std::map<int, Projectile*>* activeProjectiles) :
	AttackingActor(rocketSpeed, rocketTexture, activeProjectiles), MovingActor(1, speed, std::pair<int, int>(0, 0), std::pair<float, float>(ScreenSizeX / 2, ScreenSizeY - playerTexture->Height()), playerTexture)
{
	Health = health;
	InputHandler = inputHandler;
}

//handles boundaries
void Player::Move(float delta)
{
	float newXPos = Position.first + delta * Speed * Direction.first;
	Position.first = std::fmax(0, newXPos);
	Position.first = std::fmin(ScreenSizeX - ActorTexture->Width(), Position.first);
	Position.second += delta * Speed * Direction.second;
}

//handles input, firing rockets, and collision
void Player::Process(float delta, Grid * grid)
{
	std::list<GridEntry*> relevantEntries = grid->GetRelevantEntriesInGrid(this);

	//collision with enemies
	for each (auto entry in relevantEntries)
	{
		for each (auto enemy in entry->Enemies)
		{
			//only check enemy if they are alive
			if (enemy->IsAlive()) {
				if (this->IsCollidingWith(enemy)) {
					//notifying self that collision is ocuring
					OnCollisionEnter();

					//notifying enemy of collision
					enemy->OnCollisionEnter();
				}
			}
		}
	}

	if (Health <= 0) {
		Alive = false;
	}

	if (Alive == false)
		return;

	if (InputHandler->IsLeftDown()) {
		Direction = std::pair<int, int>(-1, 0);
	}

	else if (InputHandler->IsRightDown())
	{
		Direction = std::pair<int, int>(1, 0);
	}

	else {
		Direction = std::pair<int, int>(0, 0);
	}

	//player can only shoot twice per second
	if (timePassedSinceLastRocketFired > 0.5) {
		if (InputHandler->IsSpaceDown())
		{
			FireProjectile();
			timePassedSinceLastRocketFired = 0;
		}
	}

	timePassedSinceLastRocketFired += delta;
}

void Player::OnCollisionEnter()
{
	Health -= 1;
}

void Player::FireProjectile() {
	(*ActiveProjectiles)[ProjectileId] = new Rocket(ProjectileId, ProjectileSpeed, std::pair<int, int>(Position.first + ActorTexture->Width()*0.25, Position.second), ProjectileTexture);
	ProjectileId++;
}

void Player::TakeDamage(int damage)
{
	Health -= damage;
}

int Player::GetHealth()
{
	return Health;
}