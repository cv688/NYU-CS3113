#pragma once

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <vector>
#include <string>
#include "Entity.h"

using namespace std;

enum GameState { MAIN_MENU, GAME_STATE, GAME_OVER };

class Game {

public:
	Game();
	~Game();

	bool ProcessEvents();
	void FixedUpdate();
	void Render();

private:

	void Init();

	void buildLevel();
	
	GLuint loadTexture(const char *imagePath);

	void drawText(GLuint fontTexture, string text, float x, float y, float size,  float spacing, float r, float g, float b, float a);
	
	void Reset();
	
	float lerp(float v0, float v1, float t);

	//game variables
	int state; 
	int p1score;
	int p2score;
	int getKey; 
	int playerWin;

	//timing variables
	float lastFrameTicks;
	float timeLeftOver;
	float timeElapsed;

	const Uint8 *keys;
	SDL_Event event;
	SDL_Window *displayWindow;
	GLuint sheet;
	GLuint texture;

	//entities
	Entity player1;
	Entity player2;
	Entity lock;
	Entity key;
	vector<Entity> sprites;
};

