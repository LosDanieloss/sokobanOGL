#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <SDL.h>

class Map
{
public:
	SDL_Texture* wall;
	SDL_Texture* player;
	SDL_Texture* goal;
	SDL_Texture* chest;

private:
	SDL_Rect playerRect, goalRect, chestRect;
	std::vector<std::string> lvlList;
	int currentLvl;
	std::vector<std::string> lvlMap;
	int goalPlaces;
	int playerX, playerY;

	void readMap();
	void changeDashToSpace();
	bool moveBox(int boxX, int boxY, int dx, int dy);
	void playerFromBlank(int x, int y, int dx, int dy);
	void playerFromGoal(int x, int y, int dx, int dy);
	void boxFromBlank(int x, int y, int dx, int dy);
	void boxFromGoal(int x, int y, int dx, int dy);

public:
	Map();
	Map(SDL_Texture* wwall, SDL_Texture* pplayer, SDL_Texture* ggoal, SDL_Texture* cchest);
	~Map();
	std::string drawMap();
	void drawMapGL(void(*dispTexture)(SDL_Texture* img, SDL_Rect* srcRect, SDL_Rect* destRect));
	void nextMap();
	void prevMap();
	void resetMap();
	bool movePlayer(int dx, int dy);
	bool isWin();
};

