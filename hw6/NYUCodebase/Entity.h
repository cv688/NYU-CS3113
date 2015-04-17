#pragma once

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Matrix.h"
#define FIXED_TIMESTEP 0.0166666
#define MAX_TIMESTEPS 6

class Entity {

public:
	Entity();
	Entity(int index, int column, int row, GLuint Sprite);
	void draw();
	void update();
	void genMatrix();

	float x;
	float y;
	float u;
	float v;
	float width;
	float height;
	float speed;
	float velocity_x;
	float velocity_y;
	float s;
	float rotation;
	GLuint sprite;
	Matrix matrix;
	Vector vector;

};
