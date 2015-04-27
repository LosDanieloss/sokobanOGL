#include "Map.h"


Map::Map()
{
	currentLvl = 0;
	goalPlaces = 0;
	playerX = -1;
	playerY = -1;
	lvlList = { "lvl1", "lvl2", "lvl3", "lvl4", "lvl5" };
	readMap();
	playerRect = { 0, 0, 25, 25 };
	goalRect = { 0, 0, 25, 25 };
	chestRect = { 0, 0, 25, 25 };
	wall = NULL;
	player = NULL;
	goal = NULL;
	chest = NULL;
}

Map::Map(SDL_Texture* wwall, SDL_Texture* pplayer, SDL_Texture* ggoal, SDL_Texture* cchest){
	currentLvl = 0;
	goalPlaces = 0;
	playerX = -1;
	playerY = -1;
	lvlList = { "lvl1", "lvl2", "lvl3", "lvl4", "lvl5" };
	readMap();
	playerRect = { 0, 0, 25, 25 };
	goalRect = { 0, 0, 25, 25 };
	chestRect = { 0, 0, 25, 25 };
	wall = wwall;
	player = pplayer;
	goal = ggoal;
	chest = cchest;
	std::cout << goalPlaces << std::endl;
}


Map::~Map()
{
	SDL_DestroyTexture(wall);
	SDL_DestroyTexture(player);
	SDL_DestroyTexture(goal);
	SDL_DestroyTexture(chest);
}

std::string Map::drawMap(){
	std::string toReturn = "";
	toReturn.append(lvlList[currentLvl] + "\n");
	for (int i = 0; i < lvlMap.size(); i++){
		toReturn.append(lvlMap[i] + "\n");
	}
	return toReturn;
}

void Map::drawMapGL(void(*dispTexture)(SDL_Texture* img, SDL_Rect* srcRect, SDL_Rect* destRect)){
	SDL_Rect blabla = { 0, 0, 0, 0 };
	SDL_Texture* tmp_texture = NULL;
	SDL_Rect tmp_rect = { 0, 0, 25, 25 };
	for (int i = 0; i < lvlMap.size(); i++)
	for (int j = 0; j < lvlMap[i].length(); j++){
		if (lvlMap[i][j] == ' '){
			tmp_texture = NULL;
			tmp_rect = { 0, 0, 25, 25 };
		}
		else if (lvlMap[i][j] == '@'){
			tmp_texture = player;
			tmp_rect = playerRect;
		}
		else if (lvlMap[i][j] == '.'){
			tmp_texture = goal;
			tmp_rect = goalRect;
		}
		else if (lvlMap[i][j] == '#'){
			tmp_texture = wall;
			tmp_rect = { 0, 0, 25, 25 };
		}
		else if (lvlMap[i][j] == '$'){
			tmp_texture = chest;
			tmp_rect = chestRect;
		}
		else if (lvlMap[i][j] == '*'){
			tmp_texture = chest;
			tmp_rect = { 25, 0, 25, 25 };
		}
		else if (lvlMap[i][j] == '+'){
			tmp_texture = player;
			tmp_rect = playerRect;
		}

		if (tmp_texture != NULL){
			blabla = { ((j) * 25), ((i + 2) * 25), 25, 25 };
			dispTexture(tmp_texture, &tmp_rect, &blabla);
		}

		tmp_texture = NULL;
		SDL_DestroyTexture(tmp_texture);
	}
}

void Map::readMap(){
	goalPlaces = 0;
	lvlMap.clear();
	std::ifstream infile;
	infile.open(lvlList[currentLvl] + ".txt");
	std::string readed = "";
	while (infile >> readed)
		lvlMap.push_back(readed);
	infile.close();
	changeDashToSpace();
}

void Map::changeDashToSpace(){
	for (int i = 0; i < lvlMap.size(); i++)
	for (int j = 0; j < lvlMap[i].length(); j++)
	if (lvlMap[i][j] == '_')
		lvlMap[i][j] = ' ';
	else if (lvlMap[i][j] == '@'){
		playerX = j; playerY = i;
	}
	else if (lvlMap[i][j] == '.')
		goalPlaces++;
}

void Map::nextMap(){
	currentLvl = ++currentLvl % lvlList.size();
	readMap();
}

void Map::prevMap(){
	currentLvl = --currentLvl;
	if (currentLvl < 0)
		currentLvl = lvlList.size() - 1;
	readMap();
}

void Map::resetMap(){
	readMap();
}

bool Map::movePlayer(int dx, int dy){
	if (lvlMap[playerY + dy][playerX + dx] != '#')
	if (moveBox(playerX + dx, playerY + dy, dx, dy)){
		playerFromBlank(playerX, playerY, dx, dy);
		playerFromGoal(playerX, playerY, dx, dy);
		playerX += dx;
		playerY += dy;
		return true;
	}
	return false;
}

bool Map::moveBox(int boxX, int boxY, int dx, int dy){
	if (lvlMap[boxY][boxX] == '$' || lvlMap[boxY][boxX] == '*'){
		if (lvlMap[boxY + dy][boxX + dx] == '$' || lvlMap[boxY + dy][boxX + dx] == '#' || lvlMap[boxY + dy][boxX + dx] == '*'){
			return false;
		}
		boxFromBlank(boxX, boxY, dx, dy);
		boxFromGoal(boxX, boxY, dx, dy);
	}
	return true;
}

void Map::boxFromBlank(int x, int y, int dx, int dy){
	if (lvlMap[y][x] == '$'){
		lvlMap[y][x] = ' ';
		if (lvlMap[y + dy][x + dx] == '.'){
			lvlMap[y + dy][x + dx] = '*';
			goalPlaces--;
		}
		else
			lvlMap[y + dy][x + dx] = '$';
	}
}

void Map::boxFromGoal(int x, int y, int dx, int dy){
	if (lvlMap[y][x] == '*'){
		lvlMap[y][x] = '.';
		if (lvlMap[y + dy][x + dx] == '.'){
			lvlMap[y + dy][x + dx] = '*';
		}
		else{
			lvlMap[y + dy][x + dx] = '$';
			goalPlaces++;
		}
	}
}

void Map::playerFromBlank(int x, int y, int dx, int dy){
	if (lvlMap[y][x] == '@'){
		lvlMap[y][x] = ' ';
		if (lvlMap[y + dy][x + dx] == '.'){
			lvlMap[y + dy][x + dx] = '+';
			playerRect.x += 25;
		}
		else{
			lvlMap[y + dy][x + dx] = '@';
		}
	}
}

void Map::playerFromGoal(int x, int y, int dx, int dy){
	if (lvlMap[y][x] == '+'){
		lvlMap[y][x] = '.';
		if (lvlMap[y + dy][x + dx] == '.'){
			lvlMap[y + dy][x + dx] = '+';
		}
		else{
			lvlMap[y + dy][x + dx] = '@';
			playerRect.x -= 25;
		}
	}
}

bool Map::isWin(){
	return goalPlaces == 0 ? true : false;
}