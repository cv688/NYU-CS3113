#include "Entity.h"

Entity::Entity() {}
Entity::Entity(float x, float y, int index, int column, int row, float s, GLuint Sprite) :
sprite(Sprite), x(x), y(y), s(s)

{
	u = (float)(index % column) / (float)column;
	v = (float)(index / column) / (float)row;
	width = 1.0 / (float)column;
	height = 1.0 / (float)row;

	velocity_x = 0.0;
	velocity_y = 0.0;
	acceleration_x = 0.0;
	acceleration_y = 0.0;
	friction_x = 0.85;
	friction_y = 0.25;
}

void Entity::draw(const float &translateX, const float &translateY) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, sprite);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(translateX, translateY, 0.0);
	glTranslatef(x, y, 0.0);

	GLfloat quad[] = { width * -1.0f * s, height * s, width * -1.0f * s, height * -1.0f * s, width * s, height * -1.0f * s, width * s, height * s };

	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat quadUVs[] = { u, v, u, v + height, u + width, v + height, u + width, v };
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_QUADS, 0, 4);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

bool Entity::entityCollision(Entity object){
	//this entity's min is less than the object's max
	if (y - (height * s) < object.y + (object.height * object.s) &&
		//this entity's max is greater than the object's min
		y + (height * s) > object.y - (object.height * object.s) &&
		//this entity's min is less than the object's max,
		x - (width * s) < object.x + (object.width * object.s) &&
		//this entity's max is greater than the object's min
		x + (width * s) > object.x - (object.width * object.s)) {
		return true;
	}
	return false;
}

void Entity::jump() {
	velocity_y = 1.7;
	collideBottom = false;
}
