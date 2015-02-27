#include "SIGame.h"
using namespace std;

SpaceInvaders::SpaceInvaders() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Hw3: Space Invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 900, 700, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	glViewport(0, 0, 900, 700);
	glMatrixMode(GL_MODELVIEW);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	round = 1;
	score = 0;
	state = MAIN_MENU;
	ifpShoot = false;
	ifeShoot = false;
	lastFrameTicks = 0.0;
	ticks = (float)SDL_GetTicks() / 1000.0;
	timeElapsed = ticks - lastFrameTicks;
	keys = SDL_GetKeyboardState(NULL);

	Init(1);
}

void SpaceInvaders::Init(int round) {
	//player with speed of 0.003
	player = Sprite(0.0f, -0.95f, 0.0f / 512.0f, 420.0f / 512.0f, 72.0f / 512.0f, 70.0f / 512.0f, 0.003f, "sprites.png");

	//give player enough bullets to eliminate all enemies for several rounds and some extra for misses  
	for (int i = 0; i < (50*round); i++) {
		pBullets.push_back(Sprite(0.0, -0.8, 71.0 / 512.0, 420.0 / 512.0, 12.0 / 512.0, 20.0 / 512.0, 5.0, "sprites.png"));
	}

	//after you complete each round the amount of enemies gets mulutplied by a factor of 13
	for (int i = 0; i < (13*round); i++) {
		enemies.push_back(Sprite((-0.9 + ((i % 13) * 0.15)), 0.6, 0.0 / 512.0f, 0.0 / 512.0f, 292.0 / 512.0f, 215.0 / 512.0f, 0.0, "sprites.png"));
		if (i >= 13) {
			enemies[i].setPositionY(enemies[i].getPositionY() - 0.2);
		}
	}

	//enemies' bullets  
	for (int i = 0; i < (100*round); i++) {
		eBullets.push_back(Sprite(0.0, -0.8, 71.0 / 512.0, 420.0 / 512.0, 12.0 / 512.0, 20.0 / 512.0, 5.0, "sprites.png"));
	}

	//barriers
	for (int i = 0; i < 5; i++) {
		barriers.push_back(Sprite((-0.7 + (i * 0.35)), -0.5, 0.0 / 512.0, 218.0 / 512.0, 256.0 / 512.0, 198.0 / 512.0f, 0.0, "sprites.png"));
	}
}

//draw text 
void SpaceInvaders::DrawText(const char* image_path, string text, float x, float y, float size, float r, float g, float b, float a) {
	SDL_Surface *surface = IMG_Load(image_path);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	SDL_FreeSurface(surface);
	
	
	glBindTexture(GL_TEXTURE_2D, textureID);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLoadIdentity();
	glTranslatef(x, y, 0.0);
	vector<float> vertexData;
	vector<float> textCoordData;
	vector<float> colorData;
	float textureSize = 1.0 / 16.0f;
	for (int i = 0; i <text.size(); i++) {
		float textX = (float)(((int)text[i]) % 16) / 16.0f;
		float textY = (float)(((int)text[i]) / 16) / 16.0f;
		vertexData.insert(
			vertexData.end(), { size*i + -1 * size, size, size*i + -1 * size, -1 * size, size*i + size, -1 * size, size*i + size, size });
		colorData.insert(
			colorData.end(), { r, g, b, a, r, g, b, a, r, g, b, a, r, g, b, a });
		textCoordData.insert(
			textCoordData.end(), { textX, textY, textX, textY + textureSize, textX + textureSize, textY + textureSize, textX + textureSize, textY });
	}
	glColorPointer(4, GL_FLOAT, 0, colorData.data());
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vertexData.data());
	glEnableClientState(GL_VERTEX_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, textCoordData.data());
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDrawArrays(GL_QUADS, 0, text.size() * 4.0);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

//key events
bool SpaceInvaders::ProcessEvents() {
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			return false;
		}
	}
	if (state == MAIN_MENU){
		if (keys[SDL_SCANCODE_RETURN]) { state = GAME_STATE; }
		else if (keys[SDL_SCANCODE_ESCAPE]) { return false; }
	}
	else if (state == GAME_STATE) {
		if (keys[SDL_SCANCODE_RIGHT] && (player.getPositionX() + 0.003) < 0.925) {
			player.setPositionX(player.getPositionX() + 0.003);
		}
		else if (keys[SDL_SCANCODE_LEFT] && (player.getPositionX() + 0.003) > -0.925) {
			player.setPositionX(player.getPositionX() - 0.003);
		}
		else if (keys[SDL_SCANCODE_SPACE]) { pshoot(true); }
		else if (keys[SDL_SCANCODE_ESCAPE]) { return false; }
	}
	else if (state == GAME_OVER && keys[SDL_SCANCODE_ESCAPE]) { return false; }
	return true;
}


//enemy bullets collision with barriers
void SpaceInvaders::BarrierCollisionDetector() {
	float barrierSpace = 0.45;
	for (int i = 0; i < eBullets.size(); i++) {
		for (int j = 0; j < barriers.size(); j++) {
			if (((eBullets[i].getPositionX() > barriers[j].getPositionX() &&
				eBullets[i].getPositionX() < barriers[j].getPositionX() + barrierSpace * barriers[j].getWidth()) ||
				(eBullets[i].getPositionX() + barrierSpace * eBullets[i].getWidth() < barriers[j].getPositionX() + barrierSpace * barriers[j].getWidth() &&
				eBullets[i].getPositionX() + barrierSpace * eBullets[i].getWidth() > barriers[j].getPositionX())) &&
				((eBullets[i].getPositionY() > barriers[j].getPositionY() &&
				eBullets[i].getPositionY() < barriers[j].getPositionY() + barrierSpace * barriers[j].getHeight()) ||
				(eBullets[i].getPositionY() + barrierSpace * eBullets[i].getHeight() < barriers[j].getPositionY() + barrierSpace * barriers[j].getHeight() &&
				eBullets[i].getPositionY() + barrierSpace * eBullets[i].getHeight() > barriers[j].getPositionY())) &&
				eBullets[i].isAlive() && barriers[j].isAlive()) {

				barriers.erase(barriers.begin() + j);
				eBullets.erase(eBullets.begin() + i);
			}
		}
	}
}



void SpaceInvaders::collisionDetector() {
	//player bullets and enemies collision 
	float enemySpace = 0.3, playerSpace = 0.6;
	for (int i = 0; i < pBullets.size(); i++) { 
		for (int j = 0; j < enemies.size(); j++) {
			if (((pBullets[i].getPositionX() > enemies[j].getPositionX() &&
				pBullets[i].getPositionX() < enemies[j].getPositionX() + enemySpace * enemies[j].getWidth()) ||
				(pBullets[i].getPositionX() + enemySpace * pBullets[i].getWidth() < enemies[j].getPositionX() + enemySpace * enemies[j].getWidth() &&
				pBullets[i].getPositionX() + enemySpace * pBullets[i].getWidth() > enemies[j].getPositionX())) &&
				((pBullets[i].getPositionY() > enemies[j].getPositionY() &&
				pBullets[i].getPositionY() < enemies[j].getPositionY() + enemySpace * enemies[j].getHeight()) ||
				(pBullets[i].getPositionY() + enemySpace * pBullets[i].getHeight() < enemies[j].getPositionY() + enemySpace * enemies[j].getHeight() &&
				pBullets[i].getPositionY() + enemySpace * pBullets[i].getHeight() > enemies[j].getPositionY())) &&
				pBullets[i].isAlive() && enemies[j].isAlive()) {

				//every hit gives you 50 points
				score += 50;
				enemies.erase(enemies.begin() + j);
				pBullets.erase(pBullets.begin() + i);
			}
		}
	}

	//enemy bullets and player collision
	for (int i = 0; i < eBullets.size(); i++) { 
		if (((eBullets[i].getPositionX() > player.getPositionX() &&
			eBullets[i].getPositionX() < player.getPositionX() + playerSpace * player.getWidth()) ||
			(eBullets[i].getPositionX() + playerSpace * eBullets[i].getWidth() < player.getPositionX() + playerSpace * player.getWidth() &&
			eBullets[i].getPositionX() + playerSpace * eBullets[i].getWidth() > player.getPositionX())) &&
			((eBullets[i].getPositionY() > player.getPositionY() &&
			eBullets[i].getPositionY() < player.getPositionY() + playerSpace * player.getHeight()) ||
			(eBullets[i].getPositionY() + playerSpace * eBullets[i].getHeight() < player.getPositionY() + playerSpace * player.getHeight() &&
			eBullets[i].getPositionY() + playerSpace * eBullets[i].getHeight() > player.getPositionY())) &&
			eBullets[i].isAlive() && player.isAlive()) {

			state = GAME_OVER; 

		}
	}



}

//if bullets leave screen, remove from vector
void SpaceInvaders::bulletDetectorandUpdate() {
	for (size_t i = 0; i < pBullets.size(); i++) {
		if (pBullets[i].getPositionY() > 1.0) pBullets.erase(pBullets.begin() + i);
	}
	for (size_t i = 0; i < eBullets.size(); i++) {
		if (eBullets[i].getPositionY() < -1.0) eBullets.erase(eBullets.begin() + i);
	}

	//update bullet positions
	for (int i = 0; i < pBullets.size(); i++) {
		if (pBullets[i].isAlive() && pBullets[i].isShot()) {
			pBullets[i].setPositionY(pBullets[i].getPositionY() + (timeElapsed * 1.0));
		}
	}
	for (int i = 0; i < eBullets.size(); i++) {
		if (eBullets[i].isAlive() && eBullets[i].isShot()) {
			eBullets[i].setPositionY(eBullets[i].getPositionY() - (timeElapsed * 0.8));
		}
	}
}

//player shoot 
void SpaceInvaders::pshoot(bool tag) {
	//allow player to shoot
	if (tag && ifpShoot) {
		for (int i = 0; i < pBullets.size(); i++) {
			if (pBullets[i].isAlive() && !pBullets[i].isShot()) {
				pBullets[i].shoot();
				pBullets[i].setPositionX(player.getPositionX() + (player.getWidth()/6.0));
				pBullets[i].setPositionY(player.getPositionY() + 0.02);
				ifpShoot = false;
				break;
			}
		}
	}
}

//enemy shoot
void SpaceInvaders::eshoot(bool tag) {
	//allow enemies to shoot
	if (!tag && ifeShoot) {
		int enemyIndex = rand() % enemies.size();
		for (size_t i = 0; i < eBullets.size(); i++) {
			if (eBullets[i].isAlive() && !eBullets[i].isShot()) {
				eBullets[i].shoot();
				eBullets[i].setPositionX(enemies[enemyIndex].getPositionX());
				eBullets[i].setPositionY(enemies[enemyIndex].getPositionY() - 0.02);
				ifeShoot = false;
				break;
			}
		}
	}
}

void SpaceInvaders::Update() {
	//set up timing variables
	ticks = (float)SDL_GetTicks() / 1000.0;
	timeElapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;
	timer++;
	//timer used to update bullet firing, faster for player  
	if (!(timer % 500) && !ifpShoot) { ifpShoot = true; }
	if (!(timer % 800) && !ifeShoot) { ifeShoot = true; }

	//check for any collisions between player and enemies
	collisionDetector();

	//random enemy shoots bullet
	eshoot(false);

	//check to see if bullets left screen and update positions
	bulletDetectorandUpdate();

	//checks to see if enemy bullets hit barrier
	BarrierCollisionDetector();

	//make enemies move back and forth
	for (int i = 0; i < enemies.size(); i++){
		if (enemies[i].getPositionX() > 1.0){
			enemies[i].setDirection(-1.0);
		}
		else if (enemies[i].getPositionX() < -1.0){
			enemies[i].setDirection(1.0);
		}
		enemies[i].setPositionX(enemies[i].getPositionX() + (0.003 * enemies[i].getDirection()));
	}

	//if no more enemies are left, go to next round
	if (enemies.size() == 0) {;
		state = GAME_STATE;
		Init(round+=1);
	}

}

void SpaceInvaders::Render() {
	glClear(GL_COLOR_BUFFER_BIT);
	switch (state) {
	case MAIN_MENU:
		glClearColor(1.0, 0.0, 0.0, 0.0);
		DrawText("texture.png", "SPACE INVADERS!!!!", -0.85f, 0.1f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f);
		DrawText("texture.png", "PRESS ENTER TO PLAY", -0.9f, -0.4f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f);
		break;
	case GAME_STATE:
		glClearColor(0.0, 0.0, 0.0, 0.0);
		player.draw(0.3);
		for (size_t i = 0; i < enemies.size(); i++) {
			if (enemies[i].isAlive()) enemies[i].draw(0.2);
		}
		for (size_t i = 0; i < barriers.size(); i++) {
			if (barriers[i].isAlive()) barriers[i].draw(0.3); 
		}
		for (size_t i = 0; i < pBullets.size(); i++) {
			if (pBullets[i].isAlive() && pBullets[i].isShot()) pBullets[i].draw(0.3); break;
		}
		for (size_t i = 0; i < eBullets.size(); i++) {
			if (eBullets[i].isAlive() && eBullets[i].isShot()) eBullets[i].draw(0.6); break;
		}
		break;
	case GAME_OVER:
		glClearColor(1.0, 0.0, 0.0, 0.0);
		DrawText("texture.png", "GAME OVER", -0.8f, 0.4f, 0.2f, 1.0f, 1.0f, 1.0f, 1.0f);
		DrawText("texture.png", "FINAL SCORE: " + to_string(score), -0.8f, 0.0f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f);
		DrawText("texture.png", "PRESS ESC TO EXIT.", -0.8f, -0.4f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f);
		break;
	}
	SDL_GL_SwapWindow(displayWindow);
}
