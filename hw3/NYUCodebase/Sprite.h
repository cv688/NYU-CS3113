#pragma once 

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

class Sprite {
public:
	Sprite();
	Sprite(float x,  float y,  float u,  float v,  float w,  float h,  float s, const char *imagePath);
	
	void setDirection(const float &newDirection);
	void setPositionX(const float &newPosX);
	void setPositionY(const float &newPosY);
	void draw(float scale);
	void shoot();
	void kill();

	float getDirection();
	float getPositionX();
	float getPositionY();
	float getWidth();
	float getHeight();
	bool isAlive();
	bool isShot();

private:
	float direction = 1.0;
	float x;
	float y;
	float u;
	float v;
	float width;
	float height;
	float speed;
	GLuint texture;
	bool alive;
	bool shot;

};



