#include <iostream>
#include <stdio.h>  
#include <stdlib.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <stack>
#include "Map.h"

// defs for quick changes
#define Window_Height 700
#define Window_Wigth 1280
#define Window_Cap "Tic tac toe"

#define Frames_per_sec 30
#define Frame_rate 1000/Frames_per_sec

using namespace std;

struct StateStruct
{
	void(*StatePointer)();
};

// global vars
stack<StateStruct> game_StateStack;		// here is a stack for game states
SDL_Window* game_Window = NULL;			// game window
SDL_Surface* game_Sufrace = NULL;		// back buffer
SDL_Renderer* renderer = NULL;
SDL_Event game_Event;					// from here i can get input
int last_tick;							// timer for frame rate
Map* game_map = NULL;					// instance of our game

// Functions to handle game state
void Menu();
void Game();
void Exit();

// Function for initialization and closeing
void Init();
void CleanUp();

// Helpers for handling game states
void clearDisp();
void dispTxt(string txt, int x, int y, int size, int fR, int fG, int fB, int bR, int bG, int bB);
SDL_Texture* readImage(string name);
void dispImg(SDL_Texture* img, SDL_Rect* srcRect, SDL_Rect* destRect);
// Helpers for handling input
void handleMenuInput();
void handleGameInput();
void handleExitInput();
// Rendering game states
void renderMenu();
void renderGame();
void renderExit();
// create new game
void newGame();

int main(int argc, char** argv){

	Init();

	while (!game_StateStack.empty()){
		game_StateStack.top().StatePointer();
	}

	CleanUp();
	exit(EXIT_SUCCESS);
}

void Init(){
	// Init SDL
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

	// set up window dimension
	game_Window = SDL_CreateWindow(Window_Cap, 0, 20, Window_Wigth, Window_Height, SDL_WINDOW_SHOWN);

	// set black background
	game_Sufrace = SDL_GetWindowSurface(game_Window);
	renderer = SDL_CreateRenderer(game_Window, 0, SDL_RENDERER_ACCELERATED);
	clearDisp();

	// exit state always on bottom
	StateStruct state;
	state.StatePointer = Exit;
	game_StateStack.push(state);

	// as game start in menu so menu state must be loaded
	state.StatePointer = Menu;
	game_StateStack.push(state);

	// initialize map & give texture of wall, player, goal and chest that will be displayed in game window while in Game state
	game_map = new Map(readImage("wall.bmp"), readImage("player.bmp"), readImage("goal.bmp"), readImage("chest.bmp"));

	// Init fonts lib
	TTF_Init();

	// remember the time so can get prefer frame rate
	last_tick = SDL_GetTicks();

}

void CleanUp(){

	// fonts lib off
	TTF_Quit();

	// free surface & window
	SDL_DestroyRenderer(renderer);
	SDL_FreeSurface(game_Sufrace);
	SDL_DestroyWindow(game_Window);

	// quit sdl
	SDL_Quit();
}

void clearDisp(){
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
	SDL_RenderClear(renderer);

}

void dispTxt(string txt, int x, int y, int size, int fR, int fG, int fB, int bR, int bG, int bB){
	// prepare font
	TTF_Font* font = TTF_OpenFont("arial.ttf", 28);

	SDL_Color foreground = { fR, fG, fB };
	SDL_Color background = { bR, bG, bB };

	// temp surface for store our txt
	SDL_Surface* temp = TTF_RenderText_Blended(font, txt.c_str(), foreground);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, temp);

	// prepare and render it
	SDL_Rect rect = { x, y, 100, 100 };
	SDL_QueryTexture(texture, NULL, NULL, &(rect.w), &(rect.h));
	SDL_RenderCopy(renderer, texture, NULL, &rect);

	// always clean up after you ;-)
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(temp);
	//SDL_DestroyTexture(texture);
	TTF_CloseFont(font);

}

void Menu(){
	newGame();
	if ((SDL_GetTicks() - last_tick) >= Frame_rate){
		handleMenuInput();
		clearDisp();
		renderMenu();
		last_tick = SDL_GetTicks();
	}
}

void Game(){
	if ((SDL_GetTicks() - last_tick) >= Frame_rate){

		handleGameInput();

		clearDisp();

		renderGame();

		last_tick = SDL_GetTicks();
	}
}

void Exit(){
	if ((SDL_GetTicks() - last_tick) >= Frame_rate){
		clearDisp();
		renderExit();
		handleExitInput();
		last_tick = SDL_GetTicks();
	}
}

void handleMenuInput(){
	// if red 'x' in right corner was hit
	if (SDL_PollEvent(&game_Event)){
		if (game_Event.type == SDL_QUIT){
			while (!game_StateStack.empty())
				game_StateStack.pop();
			return;
		}
	}
	if (game_Event.type == SDL_KEYDOWN){
		// escape or e to quit game ;-)
		if (game_Event.key.keysym.sym == SDLK_e){
			game_StateStack.pop();
			return;
		}
		// p to play
		if (game_Event.key.keysym.sym == SDLK_p){
			newGame();
			StateStruct state;
			state.StatePointer = Game;
			game_StateStack.push(state);
			return;
		}
	}
}

void handleExitInput(){
	if (SDL_PollEvent(&game_Event)){
		// red x
		if (game_Event.type == SDL_QUIT){
			while (!game_StateStack.empty())
				game_StateStack.pop();
			return;
		}
		// key down
		if (game_Event.type == SDL_KEYDOWN){
			// esc or y
			if (game_Event.key.keysym.sym == SDLK_ESCAPE || game_Event.key.keysym.sym == SDLK_y){
				game_StateStack.pop();
				return;
			}
			// n so back to menu
			if (game_Event.key.keysym.sym == SDLK_n){
				StateStruct state;
				state.StatePointer = Menu;
				game_StateStack.push(state);
				return;
			}

		}
	}
}

void handleGameInput(){
	if (SDL_PollEvent(&game_Event)){
		// red x
		if (game_Event.type == SDL_QUIT){
			while (!game_StateStack.empty())
				game_StateStack.pop();
			return;
		}
		// esc so quit to menu
		if (game_Event.type == SDL_KEYDOWN){
			if (game_Event.key.keysym.sym == SDLK_ESCAPE){
				game_StateStack.pop();
				return;
			}
			// r so make new game
			if (game_Event.key.keysym.sym == SDLK_r){
				newGame();
				return;
			}
			// p for previous map
			if (game_Event.key.keysym.sym == SDLK_p){
				game_map->prevMap();
				return;
			}
			// n for next map
			if (game_Event.key.keysym.sym == SDLK_n){
				game_map->nextMap();
			}
			// movin' player around
			if (game_Event.key.keysym.sym == SDLK_UP && !game_map->isWin()){
				game_map->movePlayer(0, -1);
				return;
			}
			if (game_Event.key.keysym.sym == SDLK_DOWN && !game_map->isWin()){
				game_map->movePlayer(0, 1);
				return;
			}
			if (game_Event.key.keysym.sym == SDLK_LEFT && !game_map->isWin()){
				game_map->movePlayer(-1, 0);
				return;
			}
			if (game_Event.key.keysym.sym == SDLK_RIGHT && !game_map->isWin()){
				game_map->movePlayer(1, 0);
				return;
			}
		}
	}
}

void renderMenu(){
	dispTxt("(P)lay", Window_Wigth/2 - 100, 0, 12, 0, 0, 0, 255, 0, 0);
	dispTxt("(E)xit", Window_Wigth / 2 - 100, 50, 12, 0, 0, 0, 255, 0, 0);
	// display current game window
	SDL_RenderPresent(renderer);
}

void renderGame(){
	// set color of lines & draw lines ona a renderer
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	// display info which sign is player
	dispTxt("ESC - menu, R - new, P - prev map, N - next map", 0, 0, 1, 0, 0, 0, 255, 0, 0);
	//render game 

	// if all chest are in right place render congratz
	if (game_map->isWin())
		dispTxt("Congratz! You won!", Window_Wigth/2 - 100, Window_Height/2, 1, 0, 0, 0, 255, 0, 0);
	else
		// render game
		game_map->drawMapGL(dispImg);

	// display current game window
	SDL_RenderPresent(renderer);
}

void renderExit(){
	dispTxt("Do you wanna quit? (y) or (n)", Window_Wigth / 2 - 200, 111, 12, 0, 0, 0, 255, 0, 0);
	// display current game window
	SDL_RenderPresent(renderer);
}

void newGame(){
	game_map->resetMap();
}

SDL_Texture* readImage(string name){
	SDL_Surface* tmp_img = SDL_LoadBMP(name.c_str());
	SDL_Texture* img_txture = SDL_CreateTextureFromSurface(renderer, tmp_img);
	if (img_txture == 0)
		cout << "Blad odczytu obrazka" << endl;
	SDL_FreeSurface(tmp_img);
	return img_txture;
}

void dispImg(SDL_Texture* img, SDL_Rect* srcRect, SDL_Rect* destRect){
	SDL_RenderCopy(renderer, img, srcRect, destRect);
}