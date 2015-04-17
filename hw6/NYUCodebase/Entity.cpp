#include "Entity.h"


Entity::Entity() {}
Entity::Entity(int index, int column, int row, GLuint Sprite) : sprite(Sprite)
{
	u = (float)(index % column) / (float)column;
	v = (float)(index / column) / (float)row;
	width = 1.0 / (float)column;
	height = 1.0 / (float)row;

	x, y = 0.0;
	s = 1.0;
	rotation = 0.0;
	speed = 1.0;
	velocity_x = 0.0;
	velocity_y = 0.0;
}


void Entity::draw() {
	genMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(matrix.ml);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, sprite);

	GLfloat quad[] = { width * -1.0f * s, height * s, width * -1.0f * s, height * -1.0f * s, width * s, height * -1.0f * s, width * s, height * s };

	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat quadUVs[] = { u, v, u, v + height, u + width, v + height, u + width, v };
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glPopMatrix();
}

void Entity::genMatrix() {
	Matrix scale;
	scale.identity();
	scale.m[0][0] = s;
	scale.m[1][1] = s;

	Matrix rotate;
	rotate.identity();
	rotate.m[0][0] = cos(rotation);
	rotate.m[0][1] = sin(rotation);
	rotate.m[1][0] = -sin(rotation);
	rotate.m[1][1] = cos(rotation);

	Matrix translate;
	translate.identity();
	translate.m[3][0] = x;
	translate.m[3][1] = y;

	matrix.identity();
	matrix = scale * rotate * translate;
}

void Entity::update() {
	vector = Vector(float(cos(rotation + M_PI / 2.0f)), float(sin(rotation + M_PI / 2.0f)), 0.0f);
	vector.x *= speed;
	vector.y *= speed;
	genMatrix();

	x += velocity_x * FIXED_TIMESTEP;
	y += velocity_y * FIXED_TIMESTEP;

	if (x <= -1.4f || x >= 1.4f) {
		x = x * -0.88f;
	}
	if (y <= -1.0f || y >= 1.0f) {
		y = y* -0.88f;
	}
}
