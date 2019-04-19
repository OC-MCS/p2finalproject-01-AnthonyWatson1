#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <functional>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;
using namespace sf;
#include <string>
#include "Alien.h"
#include "Ship.h"
#include "ScreenMgr.h"
#include <list>
class TextureImpl;

//Represents a 2D texture which can be used by a Sprite to render to screen
 
class Textured
{
public:
	Textured();
	virtual ~Textured();
	//Initializes a texture with the data in the image file "fileName".
	bool LoadFromFile(const char* fileName, float scale);
	//Returns Width of the texture, 0 if not yet initialized
	 
	unsigned int Width() const;
	
	//Returns Height of the texture, 0 if not yet initialized
	unsigned int Height() const;
private:
	friend class Window;
	const std::unique_ptr<TextureImpl> m_TextureImpl;
};

//InputHandler encapsulates (very limited) keyboard input
class InputHandler
{
public:
	//True if spacebar is pressed down
	bool IsSpaceDown() const;
	//True if left arrow is pressed down
	bool IsLeftDown() const;
	//True if right arrow is pressed down
	bool IsRightDown() const;
private:
	friend class Window;
	void UpdateKeyPresses();
	bool m_SpaceDown = false;
	bool m_KeyRight = false;
	bool m_KeyLeft = false;
};

class IWindow
{
public:
	//Creates a new Game window with title windowName
	 
	static IWindow* CreateGameWindow(
		const char* windowName,
		unsigned int width,
		unsigned int height,
		function<void(IWindow*)> onInit,
		function<void()> onRender,
		function<void(float)> onUpdate);
	//Release resources associated with the game window.
	virtual void Release() = 0;
	//Drive the update loop of the game. Must be called periodically to generate callbacks onInit, onRender, onUpdate
	 
	virtual bool Update() = 0;
	//Draw the given texture at coordinates x, y (0, 0 is top left corner of window).
	 
	virtual void Draw(Texture* texture, unsigned short x, unsigned short y) = 0;
	// Draw the given text at coordinates x, y (0, 0 is top left corner of window).
	 
	virtual void Draw(const char* text, unsigned short size, unsigned short x, unsigned short y) = 0;
	//Returns a pointer to the InputHandler associated with this game window.
	virtual InputHandler* GetInputHandler() = 0;
};


int ScreenSizeX;
int ScreenSizeY;
int HighScore;
int NumEnemies;
float EnemySpeed;
float PlayerSpeed;
float ProjectileSpeed;


//necessary
map<int, MovingActor*> CastToMovingActor(map<int, Projectile*> objectsToCast) {
	map<int, MovingActor*> movingActorMap;

	map<int, Projectile*>::iterator it = objectsToCast.begin();
	for (it; it != objectsToCast.end(); ++it) {
		MovingActor* a = static_cast<MovingActor*>(&(*it->second));
		movingActorMap[a->GetId()] = a;
	}

	return movingActorMap;
}

//ugly,but necessary for downcasting
std::map<int, MovingActor*> CastToMovingActor(std::map<int, Enemy*> objectsToCast) {
	std::map<int, MovingActor*> movingActorMap;

	std::map<int, Enemy*>::iterator it = objectsToCast.begin();
	for (it; it != objectsToCast.end(); ++it) {
		MovingActor* a = static_cast<MovingActor*>(&(*it->second));
		movingActorMap[a->GetId()] = a;
	}

	return movingActorMap;
}

//The object that handles the world space etc
struct Level
{
public:
	bool initialized = false;
	float LocalEnemySpeed;
	int Score = 0;
	bool GameOver = false;

	std::map<int, Enemy*> ActiveEnemies;
	std::pair<int, int> OrigEnemyDirection = std::pair<int, int>(1, 0);
	std::pair<int, int> EnemyDirection;
	std::map<int, Projectile*> ActiveProjectiles;


	IWindow* window = nullptr;
	Texture enemy1Texture;
	Texture enemy2Texture;
	Texture enemy3Texture;
	Texture backGroundTexture;
	Texture bombTexture;
	Texture playerTexture;
	Texture rocketTexture;

	Player* player;
	Grid* grid = new Grid();

	//this function should handle loading all the textures and
	//all the enemies and their placements, according to a config file
	void OnInit(IWindow* theWindow)
	{
		//load sprites
		backGroundTexture.LoadFromFile("background.png", 1);
		enemy1Texture.LoadFromFile("alien.png", 0.3f);
		enemy2Texture.LoadFromFile("alien2.jpg", 0.3f);
		bombTexture.LoadFromFile("bomb.png", 0.3f);
		playerTexture.LoadFromFile("ship.png", 0.3f);
		rocketTexture.LoadFromFile("missile.png", 0.3f);

		window = theWindow;

		//create the enemies
		CreateEnemies();

		//creating the player
		player = new Player(3, PlayerSpeed, ProjectileSpeed, &playerTexture, &rocketTexture, window->GetInputHandler(), &ActiveProjectiles);

		initialized = true;
	}

	void OnRender()
	{
		//drawing the background
		window->Draw(&backGroundTexture, 0, 0);

		//drawing the enemies
		for each (auto enemy, ActiveEnemies)
		{
			window->Draw(enemy.second->GetTexture(), enemy.second->GetPosition().first, enemy.second->GetPosition().second);
		}

		//drawing the projectiles
		for each (auto projectile, ActiveProjectiles)
		{
			window->Draw(projectile.second->GetTexture(), projectile.second->GetPosition().first, projectile.second->GetPosition().second);
		}

		//drawing the player
		window->Draw(player->GetTexture(), player->GetPosition().first, player->GetPosition().second);

		//drawing health, score and highscore
		string stream;
		stream << "Health: " << player->GetHealth() << " Score: " << Score << " Highscore: " << std::max(Score, HighScore);
		window->Draw(stream.str().c_str(), 25, 0, 0);

	}

	void OnUpdate(float delta)
	{
		//the player has killed all enemies, so respawn them again
		if (ActiveEnemies.size() == 0)
			CreateEnemies();

		//used to determine score
		int prevActiveEnemies = ActiveEnemies.size();

		//clear the grid every frame (inneficient)
		grid->Clear();

		//add all enemies to the grid
		//wanted this to be done in the Process(...) function
		//of each object, but I ran into a cyclical reference issue
		//that I couldn't figure out...
		grid->AddEnemiesToGrid(CastToMovingActor(ActiveEnemies));
		grid->AddProjectilesToGrid(CastToMovingActor(ActiveProjectiles));
		grid->AddPlayerToGrid(player);

		//processing enemies
		for each (auto enemy in ActiveEnemies)
		{
			enemy.second->Process(delta);
		}

		//processing projectiles
		for each (auto projectile; ActiveProjectiles)
		{
			projectile.second->Process(delta, grid);
		}

		//processing player
		player->Process(delta, grid);

		//deleting enemies that have died
		for (auto it = ActiveEnemies.begin(); it != ActiveEnemies.cend();)
		{
			MovingActor* enemy = it._Ptr->_Myval.second;

			if (enemy->IsAlive() == false) {
				delete it._Ptr->_Myval.second;
				it = ActiveEnemies.erase(it);
			}

			else {
				++it;
			}
		}

		//deleting projectiles that have been destroyed
		for (auto it = ActiveProjectiles.begin(); it != ActiveProjectiles.cend();)
		{
			Projectile* projectile = it._Ptr->_Myval.second;

			if (projectile->IsAlive() == false) {
				delete it._Ptr->_Myval.second;
				it = ActiveProjectiles.erase(it);
			}

			else {
				++it;
			}
		}

		//moving enemies
		for each (auto enemy in ActiveEnemies)
		{
			enemy.second->Move(delta);
		}

		//moving projectiles
		for each (auto projectile in ActiveProjectiles)
		{
			projectile.second->Move(delta);
		}

		//moving player
		player->Move(delta);

		//determining enemy direction
		for each (auto enemy in ActiveEnemies) {
			//UpdateEnemyDirectionIfNecessary() returns true if EnemyDirection was updated
			//this is done to ensure enemy direction changes only once a frame
			if (UpdateEnemyDirectionIfNecessary(enemy.second))
				break;
		}

		//updating enemy direction and speed
		for each (auto enemy in ActiveEnemies) {
			//UpdateEnemyDirectionIfNecessary() returns true if EnemyDirection was updated
			//this is done to ensure enemy direction changes only once a frame
			enemy.second->SetDirection(EnemyDirection);
			enemy.second->SetSpeed(LocalEnemySpeed);
		}

		//updating the score
		//score increases everytime player kills an enemy
		Score += prevActiveEnemies - ActiveEnemies.size();

		if (player->IsAlive() == false)
			GameOver = true;
	}

	bool UpdateEnemyDirectionIfNecessary(MovingActor* enemy) {

		if (EnemyDirection == std::pair<int, int>(1, 0)) {
			//at right edge of window so change direction to down
			if (enemy->GetPosition().first >= ScreenSizeX - enemy1Texture.Width()) {
				EnemyDirection = std::pair<int, int>(0, 1);
				return true;
			}
		}

		//left
		else if (EnemyDirection == std::pair<int, int>(-1, 0)) {
			//at left edge of window so change direction to down
			if (enemy->GetPosition().first <= 0) {
				EnemyDirection = std::pair<int, int>(0, 1);
				return true;
			}
		}

		//down
		else if (EnemyDirection == std::pair<int, int>(0, 1)) {
			if (enemy->GetPosition().first >= ScreenSizeX - enemy1Texture.Width()) {
				LocalEnemySpeed += LocalEnemySpeed * 0.1;
				EnemyDirection = std::pair<int, int>(-1, 0);
				return true;
			}
			else if (enemy->GetPosition().first <= 0) {
				LocalEnemySpeed += LocalEnemySpeed * 0.1;
				EnemyDirection = std::pair<int, int>(1, 0);
				return true;
			}
		}

		return false;

	}

	void CreateEnemies() {
		LocalEnemySpeed = EnemySpeed;
		EnemyDirection = OrigEnemyDirection;


		int xPos = ScreenSizeX / 20, yPos = ScreenSizeY / 10, numPerRow = 0;
		Texture* textureToUseForRow = &enemy1Texture;


		for (int i = 0; i < NumEnemies; i++) {
			ActiveEnemies[i] = new Enemy(i, LocalEnemySpeed, ProjectileSpeed, std::pair<int, int>(xPos, yPos), textureToUseForRow, &bombTexture, &ActiveProjectiles);
			numPerRow++;
			xPos += enemy1Texture.Width() + ScreenSizeX / 20;

			if (xPos > ScreenSizeX - enemy1Texture.Width() || numPerRow > 4)
			{
				//every row will have a different texture than the previous row
				if (textureToUseForRow == &enemy1Texture) {
					textureToUseForRow = &enemy2Texture;
				}

				else if (textureToUseForRow == &enemy2Texture) {
					textureToUseForRow = &enemy3Texture;
				}

				else {
					textureToUseForRow = &enemy1Texture;
				}

				yPos += enemy1Texture.Height();
				xPos = ScreenSizeX / 20;
				numPerRow = 0;
			}
		}
	}
};
