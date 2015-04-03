#pragma once

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
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

	GLuint loadTexture(const char *imagePath);
	
	void drawText(GLuint fontTexture, string text, float x, float y, float size, float spacing, float r, float g, float b, float a);

	void Reset();
	
	float lerp(float v0, float v1, float t);

	void ReadFile();
	bool ReadHeader(ifstream &stream);
	bool ReadLayerData(ifstream &stream);
	bool ReadEntityData(ifstream &stream);
	void placeEntity(string &type, const float &xCoord, const float &yCoord);

	void worldCollision(const char *axis, Entity &entity);
	float tileCollision(const char *axis, const float &x, const float &y);

	void renderWorld();
	void renderTiles();

	//game variables
	int state; 
	int getKey;
	int player1Score;
	int player2Score;
	int playerWin; 

	//sound variables
	Mix_Music *backgroundMusic;
	Mix_Chunk *pickup;
	Mix_Chunk *unlock;

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
	Entity lock;
	Entity key;
	Entity player1;
	Entity player2;

	//scrolling variables
	float translateX;
	float translateY;
	float keyX, keyY;
	float lockX, lockY;
	int mapWidth;
	int mapHeight;
	unsigned char **levelData;
};

