#include "Sprite.h"

Sprite::Sprite() {}
Sprite::Sprite(float x, float y, float u, float v, float w, float h, float s, const char *imagePath) :
x(x), y(y), u(u), v(v), width(w), height(h), speed(s)

{
	
	SDL_Surface *surface = IMG_Load(imagePath);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	SDL_FreeSurface(surface);
	texture = textureID;
	alive = true; 
	shot = false; 
}

//draw Sprite
void Sprite::draw(float scale) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(x, y, 0.0);

	GLfloat quad[] = { width * -1.0f * scale, height * scale, width * -1.0f * scale, height * -1.0f * scale,
		width * scale, height * -1.0f * scale, width * scale, height * scale };

	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat quadUVs[] = { u, v, u, v + height, u + width, v + height, u + width, v };
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisable(GL_TEXTURE_2D);
}

void Sprite::setDirection(const float &newDirection) {
	direction = newDirection;
}

void Sprite::setPositionX(const float &newPosX) {
	x = newPosX; 
}
void Sprite::setPositionY(const float &newPosY) {
	y = newPosY; 
}

float Sprite::getDirection() {
	return direction;
}

float Sprite::getPositionX() { 
	return x; 
}
float Sprite::getPositionY() { 
	return y; 
}
float Sprite::getWidth() {
	return width; 
}
float Sprite::getHeight() { 
	return height; 
}

void Sprite::shoot() { 
	shot = true; 
}
void Sprite::kill() { 
	alive = false; 
}
bool Sprite::isAlive() { 
	return alive; 
}
bool Sprite::isShot() { 
	return shot; 
}