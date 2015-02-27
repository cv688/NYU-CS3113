#pragma once
using namespace std;

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include "Sprite.h"

enum GameState { MAIN_MENU, GAME_STATE, GAME_OVER};

class SpaceInvaders {

public:
	SpaceInvaders();
	bool ProcessEvents();
	void Update();
	void Render();

private:
	void Init(int round);
	SDL_Event event;
	SDL_Window *displayWindow;
	float ticks;
	float timeElapsed;
	float lastFrameTicks;
	void pshoot(bool tag);
	void eshoot(bool tag);
	void collisionDetector();
	void bulletDetectorandUpdate();
	void BarrierCollisionDetector();
	void DrawText(const char* image_path, string text, float x, float y, float size, float r, float g, float b, float a);

	int round; //used to make different levels
	int score;
	int state;
	int timer;
	bool ifpShoot;
	bool ifeShoot;
	const Uint8 *keys;

	Sprite player; 
	vector<Sprite> pBullets;
	vector<Sprite> enemies;
	vector<Sprite> eBullets;
	vector<Sprite> barriers;

};
