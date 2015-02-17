
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <iostream>

SDL_Window* displayWindow;

GLuint LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	SDL_FreeSurface(surface);

	return textureID;
}

void DrawSprite(GLint texture, float x, float y, float rotation) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(x, y, 0.0);
	glRotatef(rotation, 0.0, 0.0, 1.0);

	GLfloat quad[] = { -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f };
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat quadUVs[] = { 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0 };
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);
	glDisable(GL_TEXTURE_2D);
}


int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Hw1: 2D Design", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	bool done = false;
	
	SDL_Event event;

	//create the viewport
	glViewport(0, 0, 800, 600);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);

	//used for animation
	float lastFrameTicks = 0.0f;
	float rotation = 0.0f;

	//images
	GLuint fire2 = LoadTexture("fire2.png");
	GLuint star = LoadTexture("star.png");
	GLuint sun = LoadTexture("sun.png");

	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}

		//animation 
		float ticks = (float)SDL_GetTicks()/1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		rotation += elapsed * 120.0f;


		//set background to black
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//draw a quad
		GLfloat quad[] = { -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f };
		glVertexPointer(2, GL_FLOAT, 0, quad);
		glEnableClientState(GL_VERTEX_ARRAY);
		glDrawArrays(GL_QUADS, 0, 4);
		//alpha blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);


		//draw one big triangle
		glBegin(GL_TRIANGLES);

		glColor3f(1.0, 0.0, 0.0);
		glVertex2f(0, 0.0);

		glColor3f(0.5, 0.6, 0.7);
		glVertex2f(0.5, 0.1);

		glColor3f(0.5, 0.6, 0.7);
		glVertex2f(0.0, 0.1);

		glEnd();

		glBegin(GL_TRIANGLES);

		glColor3f(1.0, 0.0, 0.0);
		glVertex2f(0, 0.0);

		glColor3f(0.5, 0.6, 0.7);
		glVertex2f(-0.5, 0.1);

		glColor3f(0.5, 0.6, 0.7);
		glVertex2f(0.0, 0.1);

		glEnd();


		glBegin(GL_TRIANGLES);

		glColor3f(0.5, 0.6, 0.7);
		glVertex2d(0, 0.0);

		glColor3d(0.5, 0.6, 0.7);
		glVertex2d(0.5, 0.1);

		glColor3d(0.5, 0.6, 0.7);
		glVertex2d(0.0, -0.1);

		glEnd();

		glBegin(GL_TRIANGLES);

		glColor3d(0.5, 0.6, 0.7);
		glVertex2d(0, 0.0);

		glColor3d(0.5, 0.6, 0.7);
		glVertex2d(-0.5, 0.1);

		glColor3d(0.5, 0.6, 0.7);
		glVertex2d(0.0, -0.1);

		glEnd();

		// drawing the sprites
		DrawSprite(fire2, 0.5, 0.5, rotation* 1.5f);
		DrawSprite(star, -0.8, 0.35, rotation* 2.5f);
		DrawSprite(sun, 0.5, -0.5, rotation* 1.0f);
		
		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}