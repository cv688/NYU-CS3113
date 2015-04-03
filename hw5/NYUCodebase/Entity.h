#pragma once

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

class Entity {

public:
	Entity();
	Entity(float x, float y, int index, int column, int row, float s, GLuint t);

	void draw(const float &translateX, const float &translateY);
	void jump();
	bool entityCollision(Entity object);

	float x;
	float y;
	float u;
	float v;
	float width;
	float height;
	float s;
	float velocity_x;
	float velocity_y;
	float acceleration_x;
	float acceleration_y;
	float friction_x;
	float friction_y;
	bool collideBottom;
	bool collideTop;
	bool collideLeft;
	bool collideRight;
	GLuint sprite;

};
