#pragma once

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include <algorithm>
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
	void InitGame();
	void InitEntities();

	GLuint loadTexture(const char *imagePath);
	
	void drawText(GLuint font, string text, float x, float y, float size, float spacing, float r, float g, float b, float a);

	bool entityCollision(Entity entity1, Entity entity2);

	void asteroidandplayerCollision();
	
	float range(float min, float max);

	//gameplay variables
	int state;
	int timer;
	int lives;

	//timing
	float lastFrameTicks;
	float timeLeftOver;
	float elapsed;

	//entities
	Entity player;
	vector<Entity> asteroids;

	//collision
	bool pCollision;
	bool aCollision;

	//sound
	Mix_Music *backgroundMusic;
	Mix_Chunk *collide;
	Mix_Chunk *applause;
	Mix_Chunk *lose;

	const Uint8 *keys;
	SDL_Event event;
	SDL_Window *displayWindow;
	GLuint playersheet;
	GLuint texture;
	GLuint asteroidsheet;

};
