
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <iostream>

class Paddle {
public:
	Paddle(float x, float y, float w, float h, const char *imagePath) {

		SDL_Surface *surface = IMG_Load(imagePath);
		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		SDL_FreeSurface(surface);

		positionX = x;
		positionY = y;
		width = w;
		height = h;
		directionX = 0.0;
		directionY = 0.0;
		texture = textureID;

	}

	//accessors
	float getPositionX() { return positionX; }
	float getPositionY() { return positionY; }
	float getDirectionX() { return directionX; }
	float getDirectionY() { return directionY; }

	//mutators
	void setPositionX(float newX) { positionX = newX; }
	void setPositionY(float newY) { positionY = newY; }
	void setDirectionX(float newDirX) { directionX = newDirX; }
	void setDirectionY(float newDirY) { directionY = newDirY; }


	void paddleDraw() {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);

		glMatrixMode(GL_MODELVIEW);

		glLoadIdentity();
		glTranslatef(positionX, positionY, 0.0);
		glRotatef(0.0, 0.0, 0.0, 1.0);

		GLfloat quad[] = { width*-0.5f, height*0.5f, width*-0.5f, height*-0.5f, width*0.5f, height*-0.5f, width*0.5f, height*0.5f };
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


private:
	float positionX;
	float positionY;
	float width;
	float height;
	float directionX;
	float directionY;
	int texture;
};

class Ball {
public:
	Ball(float x, float y, float w, float h, float s, const char *imagePath) {

		SDL_Surface *surface = IMG_Load(imagePath);
		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		SDL_FreeSurface(surface);

		positionX = x;
		positionY = y;
		width = w;
		height = h;
		speed = s;
		directionX = 0.0;
		directionY = 0.0;
		texture = textureID;
	}
	//accessors
	float getPositionX() { return positionX; }
	float getPositionY() { return positionY; }
	float getDirectionX() { return directionX; }
	float getDirectionY() { return directionY; }
	float getSpeed() { return speed; }

	//mutators
	void setPositionX(float newX) { positionX = newX; }
	void setPositionY(float newY) { positionY = newY; }
	void setDirectionX(float newDirX) { directionX = newDirX; }
	void setDirectionY(float newDirY) { directionY = newDirY; }

	void ballDraw() {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);

		glMatrixMode(GL_MODELVIEW);

		glLoadIdentity();
		glTranslatef(positionX, positionY, 0.0);
		glRotatef(0.0, 0.0, 0.0, 1.0);

		GLfloat quad[] = { width*-0.5f, height*0.5f, width*-0.5f, height*-0.5f, width*0.5f, height*-0.5f, width*0.5f, height*0.5f };
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

	//render
	void render(SDL_Window *&displayWindow, Paddle *playerOne, Paddle *playerTwo) {

		//set up 
		glMatrixMode(GL_MODELVIEW);
		glClear(GL_COLOR_BUFFER_BIT);
		ballDraw();
		playerOne->paddleDraw();
		playerTwo->paddleDraw();
		SDL_GL_SwapWindow(displayWindow);

	}


	void updateBallMovement(const float &timeElapsed) {
		//update ball movement with time
		setPositionX(positionX + (timeElapsed * (directionX * getSpeed())));
		setPositionY(positionY + (timeElapsed * (directionY * getSpeed())));

	}

	//update ball position
	void update(Paddle *playerOne, Paddle *playerTwo, int &winCheck, const float &timeElapsed) {

		updateBallMovement(timeElapsed);

		//collision with top of the window
		if (positionY > 0.95 && directionY > 0) { setDirectionY(-1.0 * directionY); }
		//collision with bottom of the window
		else if (positionY < -0.95 && directionY < 0) { setDirectionY(-1.0 * directionY);}

		//ball hits player one's paddle
		if (positionX < -1.2 && positionY > playerOne->getPositionY() - 0.2 &&
			positionY < playerOne->getPositionY() + 0.2)
		{
			//ball increases speed when it collides with paddle one
			setDirectionX(-1.0 * directionX + 0.01);
		}
		//ball hits player two's paddle
		else if (positionX > 1.2 && positionY > playerTwo->getPositionY() - 0.2 &&
			positionY < playerTwo->getPositionY() + 0.2)
		{
			//ball increases speed when it collides with paddle two
			setDirectionX(-1.0 * directionX - 0.01);
		}

		//player one scores
		if (positionX > 1.33) {
			winCheck = 1;
			//gives slight advantage by making the starting point closer to player two
			setPositionX(0.4);
			setPositionY(0.0);
			setDirectionX(-1.0 * directionX);
		}
		//player two scores
		else if (positionX < -1.33) {
			winCheck = 2;
			//gives slight advantage by making the starting point closer to player one
			setPositionX(-0.4);
			setPositionY(0.0);
			setDirectionX(-1.0 * directionX);
		}

	}

private:
	float positionX;
	float positionY;
	float width;
	float height;
	float speed;
	float directionX;
	float directionY;
	int texture;
};


void setup(SDL_Window *&displayWindow, const char *title, const int &width, const int &height) {

	//initialize SDL
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	//create the viewport
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);

}

bool ProcessEvents(SDL_Event &event, const Uint8 *keys, Paddle *playerOne, Paddle *playerTwo, const float &timeElapsed) {

	//check to see if the game ended
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) { 
			return false;
		}
	}

	//moving player one up and down inside the window
	if (keys[SDL_SCANCODE_Q] == 1 && playerOne->getPositionY() < 0.8) {
		playerOne->setPositionY(playerOne->getPositionY() + timeElapsed * 1.5);
	}
	else if (keys[SDL_SCANCODE_A] == 1 && playerOne->getPositionY() > -0.8) {
		playerOne->setPositionY(playerOne->getPositionY() - timeElapsed * 1.5);
	}

	// moving player two up and down inside the window
	if (keys[SDL_SCANCODE_UP] == 1 && playerTwo->getPositionY() < 0.8) {
		playerTwo->setPositionY(playerTwo->getPositionY() + timeElapsed * 1.5);
	}
	else if (keys[SDL_SCANCODE_DOWN] == 1 && playerTwo->getPositionY() > -0.8) {
		playerTwo->setPositionY(playerTwo->getPositionY() - timeElapsed * 1.5);
	}

	return true;

}


int main(int argc, char *argv[])
{
	SDL_Window *displayWindow = NULL;
	SDL_Event event;
	setup(displayWindow, "Hw 2: Pong", 800, 600);

	//animation with time
	float lastFrameTicks = 0.0;
	float ticks = (float)SDL_GetTicks() / 1000.0;
	float timeElapsed = ticks - lastFrameTicks;

	int lastWinner = 0;
	float winTime = -450.0;

	//initialize both players and the ball
	Paddle *playerOne = new Paddle(-1.3, 0.0, 0.1, 0.3, "paddle.png");
	Paddle *playerTwo = new Paddle(1.3, 0.0, 0.1, 0.3, "paddle.png");
	Ball *ball = new Ball(0.0, 0.0, 0.1, 0.1, 6.2, "ball.png");

	int winCheck = 0;
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	//move the ball to start
	ball->setDirectionX(0.09);
	ball->setDirectionY(-0.09);


	while (ProcessEvents(event, keys, playerOne, playerTwo, timeElapsed)) {
		ticks = (float)SDL_GetTicks()/1000.0;
		timeElapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		ball->update(playerOne, playerTwo, winCheck, timeElapsed);

		//check to see which player won
		if (winCheck > 0) {
			winTime = lastFrameTicks;
			lastWinner = winCheck;
			winCheck = 0;
		}

		//set screen to black
		glClearColor(0.0, 0.0, 0.0, 0.0);

		if (winTime + 0.5 > lastFrameTicks){
			//if player one wins, the screen flashes blue for 0.5 seconds
			if (lastWinner == 1)  
				glClearColor(0.0, 0.0, 0.5, 0.0);
			//if player two wins, the screen flashes green for 0.5 seconds
			else if (lastWinner == 2)  
				glClearColor(0.0, 0.5, 0.0, 0.0);
		}

		ball->render(displayWindow, playerOne, playerTwo);
		
	}
	SDL_Quit();
	return 0;
}

