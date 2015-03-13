#include "Game.h"
#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6

Game::Game() {
	Init();
	buildLevel();
}

Game::~Game() { SDL_Quit(); }

void Game::Init() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Hw 4: Simple Platformer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 900, 700, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	glViewport(0, 0, 900, 700);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);

	state = MAIN_MENU;
	lastFrameTicks = 0.0;
	timeLeftOver = 0.0;
	sheet = loadTexture("sprite.png");
	texture = loadTexture("texture.png");

	keys = SDL_GetKeyboardState(NULL);
	p1score = 0;
	p2score = 0;
	getKey = 0;
	playerWin = 0;

	//initialize player1
	player1 = Entity(sheet, -1.0, 0.25, 239, 30, 30, 3.0);
	//initialize player2
	player2 = Entity(sheet, 1.0, 0.25, 299, 30, 30, 3.0);
	//intialize key
	key = Entity(sheet, 0.0, 0.8, 406, 30, 30, 3.0);
	//initialize lock
	lock = Entity(sheet, 0.0, -0.73, 225, 30, 30, 3.0);
}


void Game::buildLevel() {

	//floor
	for (int i = 0; i < 17; i++) {
		sprites.push_back(Entity(sheet, (-1.25 + (i * 0.16)), -0.91, 121, 30, 30, 3.0));
	}
	//platforms
	for (int i = 0; i < 3; i++) {
		sprites.push_back(Entity(sheet, (-0.65 + (i * 0.17)), -0.35, 128, 30, 30, 3.0));
	}
	for (int i = 0; i < 3; i++) {
		sprites.push_back(Entity(sheet, (0.55 - (i * 0.17)), -0.35, 128, 30, 30, 3.0));
	}
	for (int i = 0; i < 5; i++) {
		sprites.push_back(Entity(sheet, (-0.4 + (i * 0.17)), -0.1, 128, 30, 30, 3.0));
	}
	for (int i = 0; i < 3; i++) {
		sprites.push_back(Entity(sheet, (-1.35 + (i * 0.17)), 0.35, 128, 30, 30, 3.0));
	}
	for (int i = 0; i < 3; i++) {
		sprites.push_back(Entity(sheet, (1.0 + (i * 0.17)), 0.35, 128, 30, 30, 3.0));
	}
	for (int i = 0; i < 1; i++) {
		sprites.push_back(Entity(sheet, (0.0 + (i * 0.17)), 0.65, 128, 30, 30, 3.0));
	}

}



GLuint Game::loadTexture(const char *imagePath) {
	SDL_Surface *surface = IMG_Load(imagePath);
	if (!surface){ return -1; }
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	SDL_FreeSurface(surface);
	return textureID;
}

void Game::drawText(GLuint font, string text, float x, float y, float size, float spacing, float r, float g, float b, float a) {
	glBindTexture(GL_TEXTURE_2D, font);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLoadIdentity();
	glTranslatef(x, y, 0.0);
	float texture_size = 1.0 / 16.0f;
	vector<float> vertexData;
	vector<float> texCoordData;
	vector<float> colorData;

	for (size_t i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;

		colorData.insert(colorData.end(), { r, g, b, a, r, g, b, a, r, g, b, a, r, g, b, a });

		vertexData.insert(vertexData.end(), { ((size + spacing) * i) + (-0.5f * size), 0.5f * size, ((size + spacing) * i) +
			(-0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), 0.5f
			* size });

		texCoordData.insert(texCoordData.end(), { texture_x, texture_y, texture_x, texture_y + texture_size, texture_x +
			texture_size, texture_y + texture_size, texture_x + texture_size, texture_y });
	}

	glColorPointer(4, GL_FLOAT, 0, colorData.data());
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vertexData.data());
	glEnableClientState(GL_VERTEX_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, texCoordData.data());
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDrawArrays(GL_QUADS, 0, text.size() * 4);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


void Game::Reset() {
	//reset the lock and key
	key = Entity(sheet, 0.0, 0.8, 406, 30, 30, 3.0);
	lock = Entity(sheet, 0.0, -0.73, 225, 30, 30, 3.0);
	getKey = 0;
}

float Game::lerp(float v0, float v1, float t) {
	return (1.0 - t) * v0 + t * v1;
}

bool Game::ProcessEvents() {
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			return false;
		}
	}

	float ticks = (float)SDL_GetTicks() / 1000.0;
	timeElapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;
	float fixedElapsed = timeElapsed + timeLeftOver;
	if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
		fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
	}
	while (fixedElapsed >= FIXED_TIMESTEP) {
		fixedElapsed -= FIXED_TIMESTEP;
		FixedUpdate();
		Render();
	}
	timeLeftOver = fixedElapsed;

	if (state == MAIN_MENU){
		if (keys[SDL_SCANCODE_RETURN]) {
			state = GAME_STATE;
		}
		else if (keys[SDL_SCANCODE_ESCAPE]) {
			return false;
		}
	}
	else if (state == GAME_STATE) {

		if (keys[SDL_SCANCODE_ESCAPE]) {
			return false;
		}

		//player1 x-axis acceleration
		if (keys[SDL_SCANCODE_S]) { //player1 move left
			player1.acceleration_x = -2.0;
		}
		else if (keys[SDL_SCANCODE_F]) { //player1 move right
			player1.acceleration_x = 2.0;
		}
		else {
			player1.acceleration_x = 0.0;
		}

		//player2 x-axis acceleration
		if (keys[SDL_SCANCODE_LEFT]) { //player2 move left
			player2.acceleration_x = -2.0;
		}
		else if (keys[SDL_SCANCODE_RIGHT]) { //player2 move right
			player2.acceleration_x = 2.0;
		}
		else {
			player2.acceleration_x = 0.0;
		}

		//player1 jump
		if (keys[SDL_SCANCODE_E] && player1.collideBottom == true) {
			player1.jump();
		}

		//player2 jump
		if (keys[SDL_SCANCODE_UP] && player2.collideBottom == true) {
			player2.jump();
		}

	}
	//end game
	else if (state == GAME_OVER && keys[SDL_SCANCODE_ESCAPE]) {
		return false;
	}
	return true;
}

void Game::FixedUpdate() {

	//if player moves off the screen, opposing player wins
	if (player1.x > 1.5 || player1.x < -1.5) { 
		playerWin = 2;
		state = GAME_OVER;
	}
	if (player2.x > 1.5 || player2.x < -1.5) { 
		playerWin = 1;
		state = GAME_OVER;
	}


	//if either of the players have picked up the key, redraw it and set the flag
	if ((player1.entityCollision(key) || player2.entityCollision(key)) && getKey == 0) {
		key = Entity(sheet, 0.0, 0.8, 406, 30, 30, 3.0);
		if (player1.entityCollision(key)) {getKey = 1;}
		if (player2.entityCollision(key)) {getKey = 2;}
	}

	//if either of the players have unlocked the door, redraw it and set the flag
	if ((key.entityCollision(lock) && getKey == 0)) {
		lock = Entity(sheet, 0.0, -0.73, 225, 30, 30, 3.0);
		if (player1.entityCollision(key)) { getKey = 1; }
		if (player2.entityCollision(key)) { getKey = 2; }
	}


	//update the positions of the key if they are picked up
	if (getKey == 1) { 
		if (key.entityCollision(lock)) {
			key = Entity(sheet, 0.0, 0.8, 405, 30, 30, 3.0);
			lock = Entity(sheet, 0.0, -0.73, 224, 30, 30, 3.0);
			getKey = 0;
			p1score += 50;
		}
		else {
			key.x = (player1.x + (player1.width * player1.s * 0.5));
			key.y = player1.y;
		}
	}

	if (getKey == 2) {
		if (key.entityCollision(lock)) {
			key = Entity(sheet, 0.0, 0.8, 405, 30, 30, 3.0);
			lock = Entity(sheet, 0.0, -0.73, 224, 30, 30, 3.0);
			getKey = 0;
			p2score += 50;
		}
		else {
			key.x = (player2.x + (player2.width * player2.s * 0.5));
			key.y = player2.y;
		}
	}

	//reset the collisions 
	player1.collideBottom = false;
	player2.collideBottom = false;

	//player1 gravity
	if (player1.collideBottom != true ) player1.acceleration_y = -2.5;

	//player1 y-axis velocity and collision detection
	player1.velocity_y = lerp(player1.velocity_y, 0.0, FIXED_TIMESTEP * player1.friction_y);
	player1.velocity_y = (player1.velocity_y + (player1.acceleration_y * FIXED_TIMESTEP));
	player1.y = (player1.y + (player1.velocity_y * FIXED_TIMESTEP));

	for (size_t i = 0; i < sprites.size(); i++) {
		if (player1.entityCollision(sprites[i])) {
			float yCol = fabs(fabs(player1.y - sprites[i].y) - (player1.height * player1.s) - (sprites[i].height * sprites[i].s));
			if (player1.y > sprites[i].y) {
				player1.y = (player1.y + (yCol + 0.000001));
			}
			else { player1.y = (player1.y - (yCol + 0.000001)); }
			player1.velocity_y = 0.0;
		}
	}

	//player1 x-axis velocity and collision detection
	player1.velocity_x = lerp(player1.velocity_x, 0.0, FIXED_TIMESTEP * player1.friction_x);
	player1.velocity_x = (player1.velocity_x + (player1.acceleration_x * FIXED_TIMESTEP));
	player1.x = (player1.x + (player1.velocity_x * FIXED_TIMESTEP));

	//player1 collisions with world sprites
	for (size_t i = 0; i < sprites.size(); i++) {
		if (player1.entityCollision(sprites[i])) {
			float xCol = fabs(fabs(player1.x - sprites[i].x) - (player1.width * player1.s) - (sprites[i].width * sprites[i].s));
			if (player1.x > sprites[i].x) {
				player1.x = (player1.x + (xCol + 0.000001));
			}
			else { player1.x = (player1.x - (xCol + 0.000001)); }
			player1.velocity_x = 0.0;
		}
	}

	//player2 gravity
	if (player2.collideBottom != true) player2.acceleration_y = -2.5;

	//player2 y-axis velocity and collision detection
	player2.velocity_y = lerp(player2.velocity_y, 0.0, FIXED_TIMESTEP * player2.friction_y);
	player2.velocity_y = (player2.velocity_y + (player2.acceleration_y * FIXED_TIMESTEP));
	player2.y = (player2.y + (player2.velocity_y * FIXED_TIMESTEP));

	for (size_t i = 0; i < sprites.size(); i++) {
		if (player2.entityCollision(sprites[i])) {
			float yCol = fabs(fabs(player2.y - sprites[i].y) - (player2.height * player2.s) - (sprites[i].height * sprites[i].s));
			if (player2.y > sprites[i].y) {
				player2.y = (player2.y + (yCol + 0.000001));
			}
			else { player2.y = (player2.y - (yCol + 0.000001)); }
			player2.velocity_y = 0.0;
		}
	}

	//player2 x-axis velocity and collision detection
	player2.velocity_x = lerp(player2.velocity_x, 0.0, FIXED_TIMESTEP * player2.friction_x);
	player2.velocity_x = (player2.velocity_x + (player2.acceleration_x * FIXED_TIMESTEP));
	player2.x = (player2.x + (player2.velocity_x * FIXED_TIMESTEP));

	//player2 collisions with world sprites
	for (size_t i = 0; i < sprites.size(); i++) {
		if (player2.entityCollision(sprites[i])) {
			float xCol = fabs(fabs(player2.x - sprites[i].x) - (player2.width * player2.s) - (sprites[i].width * sprites[i].s));
			if (player2.x > sprites[i].x) {
				player2.x = (player2.x + (xCol + 0.000001));
			}
			else { player2.x = (player2.x - (xCol + 0.000001)); }
			player2.velocity_x = 0.0;
		}
	}

	//player1 wins
	if (p1score >= 300) {
		playerWin = 1;
		state = GAME_OVER;
	}
	//player2 wins
	else if (p2score >= 300) {
		playerWin = 2;
		state = GAME_OVER;
	}

}

void Game::Render() {
	glClear(GL_COLOR_BUFFER_BIT);
	switch (state) {
	case MAIN_MENU:
		drawText(texture, "UNLOCK THE DOOR!", -0.8f, 0.7f, 0.1f, 0.0, 1.0f, 0.0f, 0.0f, 1.0f);
		drawText(texture, "P1: S left, F right, E jump ", -1.25f, 0.4f, 0.06f, 0.0125, 1.0f, 1.0f, 1.0f, 1.0f);
		drawText(texture, "P2: LEFT left, RIGHT right, UP jump ", -1.25f, 0.3f, 0.06f, 0.0125, 1.0f, 1.0f, 1.0f, 1.0f);
		drawText(texture, "Get 300 points to win!", -1.25, 0.2, 0.06, 0.0125, 1.0, 1.0, 1.0, 1.0);
		drawText(texture, "PRESS ENTER TO START!", -1.1, -0.4, 0.1, 0.0125, 1.0, 0.0, 0.0, 1.0);
		break;
	case GAME_STATE:
		//player1 and player2 scores
		drawText(texture, to_string(p1score), -0.85, 0.9, 0.09, 0.0125, 0.0, 1.0, 0.0, 1.0);
		drawText(texture, to_string(p2score), 0.85, 0.9, 0.09, 0.0125, 1.0, 0.5, 0.0, 1.0);

		//draw players
		player1.draw();
		player2.draw();

		//draw the key and lock
		key.draw();
		lock.draw();

		//draw the world sprites
		for (size_t i = 0; i < sprites.size(); i++) sprites[i].draw();
		break;
	case GAME_OVER:
		drawText(texture, "GAME OVER", -0.85f, 0.7f, 0.2f, 0.0, 1.0f, 0.0f, 0.0f, 1.0f);
		//player1 wins
		if (playerWin == 1) {
			drawText(texture, "PLAYER1 WINS!", -1.2f, 0.2f, 0.2f, 0.0, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		//player2 wins
		else if (playerWin == 2) {
			drawText(texture, "PLAYER2 WINS!", -1.2f, 0.2f, 0.2f, 0.0, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		drawText(texture, "PRESS ESC TO EXIT", -0.85f, -0.4f, 0.1f, 0.0, 1.0f, 1.0f, 1.0f, 1.0f);
		break;
	}
	SDL_GL_SwapWindow(displayWindow);


	
}