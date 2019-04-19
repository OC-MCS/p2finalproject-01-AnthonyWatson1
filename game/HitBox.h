#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;
#include <map>
#include <list>
#include <algorithm>
#include "ScreenMgr.h"
class MovingActor;

int rowSize;
int colSize;

//Grid object which greatly simplifies collision.
//No matter the resolution, the world space is divided
//into 100 sections, and an object only has to check 
//it's local sections for overlapping objects
class Grid
{
private:
	map<pair<int, int>, GridEntry*> grid;

public:
	Grid()
	{

	}

	list<GridEntry*> GetRelevantEntriesInGrid(MovingActor* object);
	void AddEnemiesToGrid(map<int, MovingActor*> objectsToAdd);
	void AddProjectilesToGrid(map<int, MovingActor*> objectsToAdd);
	void AddPlayerToGrid(MovingActor* playerObject);
	void Clear();

	~Grid()
	{

	}
};

//Object that represents a "section" of the grid.
//The Enemies and Projectiles lists are checked by
// respective actors for collisions.
//For example, the rocket class only checks
//the Enemies list to determine if it has 
//collided with something meaningful
class GridEntry
{
public:

	list<MovingActor*> Enemies;
	list<MovingActor*> Projectiles;
	MovingActor* PlayerActor;

public:

	GridEntry()
	{

	}

	void Clear();

	~GridEntry()
	{

	}
};

//Grid Functions
static const pair<int, int> unitFraction = pair<int, int>(10, 10);

Grid::Grid()
{
	std::pair<int, int> unitSize = std::pair<int, int>(ScreenSizeX / unitFraction.first, ScreenSizeY / unitFraction.second);

	for (int i = 0; unitSize.first*i < ScreenSizeX; i++) {
		for (int j = 0; unitSize.second*j < ScreenSizeY; j++) {
			grid[std::pair<int, int>(i, j)] = new GridEntry();
		}
	}
}

list<GridEntry*> Grid::GetRelevantEntriesInGrid(MovingActor* actor)
{

	pair<int, int> startGridPos = pair<int, int>(actor->GetPosition().first / ScreenSizeX * unitFraction.first, actor->GetPosition().second / ScreenSizeY * unitFraction.second);
	pair<int, int> endGridPos = pair<int, int>(((actor->GetPosition().first + actor->GetTexture()->Width()) / ScreenSizeX * unitFraction.first) + 1,
		((actor->GetPosition().second + actor->GetTexture()->Height()) / ScreenSizeY * unitFraction.second) + 1);


	startGridPos.first = max(0, startGridPos.first);
	startGridPos.first = min(unitFraction.first - 1, startGridPos.first);

	startGridPos.second = max(0, startGridPos.second);
	startGridPos.second = min(unitFraction.first - 1, startGridPos.second);

	endGridPos.first = max(0, endGridPos.first);
	endGridPos.first = min(unitFraction.first - 1, endGridPos.first);


	endGridPos.second = max(0, endGridPos.second);
	endGridPos.second = min(unitFraction.first - 1, endGridPos.second);

	list<GridEntry*> relevantEntries;

	for (int i = startGridPos.first; i <= endGridPos.first; i++) {
		for (int j = startGridPos.second; j <= endGridPos.second; j++) {
			relevantEntries.push_back(grid[pair<int, int>(i, j)]);
		}
	}

	return relevantEntries;
}

void Grid::AddEnemiesToGrid(map<int, MovingActor*> objectsToAdd)
{
	for each (object; objectsToAdd)
	{
		list<GridEntry*> entries = GetRelevantEntriesInGrid(object.second);
		for each (auto entry, entries)
		{
			entry->Enemies.push_back(object.second);
		}
	}
}

void Grid::AddProjectilesToGrid(std::map<int, MovingActor*> objectsToAdd)
{
	for each (auto object, objectsToAdd)
	{
		list<GridEntry*> entries = GetRelevantEntriesInGrid(object.second);
		for each (auto entry, entries)
		{
			entry->Projectiles.push_back(object.second);
		}
	}
}

void Grid::AddPlayerToGrid(MovingActor * playerObject)
{
	std::list<GridEntry*> entries = GetRelevantEntriesInGrid(playerObject);
	for each (auto entry in entries)
	{
		entry->PlayerActor = playerObject;
	}
}

void Grid::Clear()
{
	for each (auto entry in grid)
	{
		entry.second->Clear();
	}
}

//Grid entry functions
void GridEntry::Clear()
{
	Enemies.clear();
	Projectiles.clear();
	PlayerActor = nullptr;
}