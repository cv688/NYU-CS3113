#include "Game.h"

Game::Game() {
	InitGame();
	InitEntities();
}

void Game::InitGame() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Hw 6: Asteroids!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 900, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	glViewport(0, 0, 900, 600);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0, 0.0, 0.0, 0.0);

	state = MAIN_MENU;
	keys = SDL_GetKeyboardState(NULL);
	texture = loadTexture("texture.png");
	playersheet = loadTexture("sheet.png");
	asteroidsheet = loadTexture("sprite.png");
	lastFrameTicks = 0.0;
	timeLeftOver = 0.0;

	//sound
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	backgroundMusic = Mix_LoadMUS("Kalimba.mp3");
	//repeat until game is stopped 
	Mix_PlayMusic(backgroundMusic, -1);
	//sound effect for player collision with asteroid
	collide = Mix_LoadWAV("collide.wav");
	//sound effect when you win
	applause = Mix_LoadWAV("applause.wav");
	//sound effect when you lose
	lose = Mix_LoadWAV("trombone.wav");

	pCollision = false;
	aCollision = false;

	lives = 10;

	//about 15 seconds
	timer = 1000;
}

void Game::InitEntities() {

	//initialize player at origin
	player = Entity(5, 15, 15, playersheet);
	player.x = 0.0;
	player.y = 0.0;
	player.s = 0.8;
	player.rotation = 0.0;
	player.velocity_x = 0.0;
	player.velocity_y = 0.0;

	//initialize asteroids at random positions with random scales, rotations, and velocities
	for (int i = 0; i < 20; i++) {
		Entity asteroid = Entity(3507, 30, 30, asteroidsheet);
		asteroid.x = range(-1.0, 1.0);
		asteroid.y = range(-1.1, 1.1);
		asteroid.s = 1.4 * range(0.5, 1.1);
		asteroid.rotation = range(0.0, 2.0 * (float)M_PI);
		asteroid.velocity_x = range(-0.3, 0.3 * (float)M_PI);
		asteroid.velocity_y = range(-0.3, 0.3 * (float)M_PI);
		asteroids.push_back(asteroid);
	}
}

Game::~Game() {
	Mix_FreeMusic(backgroundMusic);
	Mix_FreeChunk(collide);
	Mix_FreeChunk(applause);
	Mix_FreeChunk(lose);
	SDL_Quit();
}

GLuint Game::loadTexture(const char *imagePath) {
	SDL_Surface *surface = IMG_Load(imagePath);
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
	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	float texture_size = 1.0 / 16.0;
	vector<float> vertexData;
	vector<float> texCoordData;
	vector<float> colorData;

	for (size_t i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		colorData.insert(colorData.end(), { r, g, b, a, r, g, b, a, r, g, b, a, r, g, b, a });
		vertexData.insert(vertexData.end(),
		{ ((size + spacing) * i) + (-0.5f * size), 0.5f * size, ((size + spacing) * i) + (-0.5f * size), -0.5f * size, ((size + spacing) * i)
		+ (0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), 0.5f * size });
		texCoordData.insert(texCoordData.end(), { texture_x, texture_y, texture_x, texture_y + texture_size, texture_x + texture_size,
			texture_y + texture_size, texture_x + texture_size, texture_y });
	}

	glPushMatrix();
	glTranslatef(x, y, 0.0);
	glColorPointer(4, GL_FLOAT, 0, colorData.data());
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vertexData.data());
	glEnableClientState(GL_VERTEX_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, texCoordData.data());
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDrawArrays(GL_QUADS, 0, text.size() * 4);

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

float Game::range(float min, float max){
	float r = ((float)rand()) / (float)RAND_MAX;
	return ((r*(max - min)) + min);
}

bool Game::ProcessEvents() {
	float ticks = (float)SDL_GetTicks() / 1000.0;
	elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			return false;
		}
	}

	float fixedElapsed = elapsed + timeLeftOver;
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
		if (keys[SDL_SCANCODE_UP]) {
			player.x += 0.8 * player.vector.x * elapsed;
			player.y += 0.8 * player.vector.y * elapsed;
		}
		if (keys[SDL_SCANCODE_DOWN]) {
			player.x -= 0.8 * player.vector.x * elapsed;
			player.y -= 0.8 * player.vector.y * elapsed;
		}
		if (keys[SDL_SCANCODE_LEFT]) {
			player.rotation += 5.0 * elapsed;
		}
		if (keys[SDL_SCANCODE_RIGHT]) {
			player.rotation -= 5.0 * elapsed;
		}
	}

	//end game
	else if (state == GAME_OVER && keys[SDL_SCANCODE_ESCAPE]) {
		return false;
	}
	return true;
}

void Game::FixedUpdate() {
	pCollision = false;
	aCollision = false;

	player.update();

	asteroidandplayerCollision();

	//if you have no lives left, you lose
	if (lives <= 0) {
		state = GAME_OVER;
	}

	//decrease timer
	if (state == GAME_STATE) {
		timer -= 0.1;
	}

	//if the timer runs out, you have won
	if (timer <= 0) {
		state = GAME_OVER;
	}
}

bool Game::entityCollision(Entity entity1, Entity entity2) {

	//check to see if it is the same entity, no collision
	if (&entity1 == &entity2) {
		return false;
	}

	//generate the matrices 
	entity1.genMatrix();
	entity2.genMatrix();

	//get the inverse of each entity's transform matrix
	Matrix ent1Inverse = entity1.matrix.inverse();
	Matrix ent2Inverse = entity2.matrix.inverse();

	//calculate the width and height of each entity
	float ent1Width = entity1.width * entity1.s * 0.9f;
	float ent1Height = entity1.height * entity1.s * 0.9f;
	float ent2Width = entity2.width * entity2.s * 0.9f;
	float ent2Height = entity2.height * entity2.s * 0.9f;

	//create vector for each corner of entity1
	Vector ent1TL = Vector(-ent1Width, ent1Height, 0.0);
	Vector ent1BL = Vector(-ent1Width, -ent1Height, 0.0);
	Vector ent1BR = Vector(ent1Width, -ent1Height, 0.0);
	Vector ent1TR = Vector(ent1Width, ent1Height, 0.0);

	//multiply entity1's vector by the inverse of entity2's transform matrix
	ent1TL = entity1.matrix * ent1TL;
	ent1BL = entity1.matrix * ent1BL;
	ent1BR = entity1.matrix * ent1BR;
	ent1TR = entity1.matrix * ent1TR;

	ent1TL = ent2Inverse * ent1TL;
	ent1BL = ent2Inverse * ent1BL;
	ent1BR = ent2Inverse * ent1BR;
	ent1TR = ent2Inverse * ent1TR;

	//use min and max of entity1 for collision with entity2
	float minX, maxX, minY, maxY;
	minX = min(min(min(ent1TL.x, ent1BL.x), ent1TR.x), ent1BR.x);
	maxX = max(max(max(ent1TL.x, ent1BL.x), ent1TR.x), ent1BR.x);
	if (!(minX <= ent2Width && maxX >= -ent2Width)) {
		return false;
	}
	minY = min(min(min(ent1TL.y, ent1BL.y), ent1TR.y), ent1BR.y);
	maxY = max(max(max(ent1TL.y, ent1BL.y), ent1TR.y), ent1BR.y);
	if (!(minY <= ent2Height && maxY >= -ent2Height)) {
		return false;
	}

	//create vector for each corner of entity2
	Vector ent2TL = Vector(-ent2Width, ent2Height, 0.0);
	Vector ent2BL = Vector(-ent2Width, -ent2Height, 0.0);
	Vector ent2BR = Vector(ent2Width, -ent2Height, 0.0);
	Vector ent2TR = Vector(ent2Width, ent2Height, 0.0);

	//multiply entity2's vector by the inverse of entity1's transform matrix
	ent2TL = entity2.matrix * ent2TL;
	ent2BL = entity2.matrix * ent2BL;
	ent2TR = entity2.matrix * ent2TR;
	ent2BR = entity2.matrix * ent2BR;

	ent2TL = ent1Inverse * ent2TL;
	ent2BL = ent1Inverse * ent2BL;
	ent2BR = ent1Inverse * ent2BR;
	ent2TR = ent1Inverse * ent2TR;

	//use min and max of entity2 for collision with entity1
	minX = min(min(min(ent2TL.x, ent2BL.x), ent2TR.x), ent2BR.x);
	maxX = max(max(max(ent2TL.x, ent2BL.x), ent2TR.x), ent2BR.x);
	if (!(minX <= ent1Width && maxX >= -ent1Width)) {
		return false;
	}
	minY = min(min(min(ent2TL.y, ent2BL.y), ent2TR.y), ent2BR.y);
	maxY = max(max(max(ent2TL.y, ent2BL.y), ent2TR.y), ent2BR.y);
	if (!(minY <= ent1Height && maxY >= -ent1Height)) {
		return false;
	}

	return true;

}

void Game::asteroidandplayerCollision() {
	for (size_t i = 0; i < asteroids.size(); i++) {
		asteroids[i].update();

		//Update position of asteroids, wrap around if they go off screen
		if (asteroids[i].x <= -1.4f || asteroids[i].x >= 1.4f) {
			asteroids[i].x = -0.9f;
		}
		if (asteroids[i].y <= -1.0f || asteroids[i].y >= 1.0f) {
			asteroids[i].y *= -0.9f;
		}

		//asteroid and player collision 
		if (entityCollision(player, asteroids[i])) {
			pCollision = true;
			Vector distance = Vector(player.x - asteroids[i].x, player.y - asteroids[i].y, 0.0);
			float dLen = distance.length();
			distance.normalize();

			player.x += distance.x * 0.02f;
			player.y += distance.y * 0.02f;

			asteroids[i].x -= distance.x * 0.02f;
			asteroids[i].y -= distance.y * 0.02f;

			if (state == GAME_STATE) {
				//if player collides with an asteroid, you lose a life
				lives -= 1;
				Mix_PlayChannel(-1, collide, 0);
			}
		}

		//asteroid and asteroid collision
		for (size_t a = 0; a < asteroids.size(); a++) {
			if (entityCollision(asteroids[i], asteroids[a])) {
				aCollision = true;
				Vector distance = Vector(asteroids[i].x - asteroids[a].x, asteroids[i].y - asteroids[a].y, 0.0);
				float dLen = distance.length();
				distance.normalize();

				asteroids[i].x += distance.x * 0.02f;
				asteroids[i].y += distance.y * 0.02f;

				asteroids[a].x -= distance.x * 0.02f;
				asteroids[a].y -= distance.y * 0.02f;
			}
		}
	}
}

void Game::Render() {
	glClear(GL_COLOR_BUFFER_BIT);
	switch (state) {
	case MAIN_MENU:
		drawText(texture, "ASTEROIDS!", -0.7f, 0.7f, 0.15f, 0.0, 1.0f, 0.0f, 0.0f, 1.0f);
		drawText(texture, "Directions: ", -0.5f, 0.4f, 0.06f, 0.0125, 1.0f, 1.0f, 1.0f, 1.0f);
		drawText(texture, "Left: <- ", -0.5f, 0.3f, 0.06f, 0.0125, 1.0f, 1.0f, 1.0f, 1.0f);
		drawText(texture, "Right: -> ", -0.5f, 0.2f, 0.06f, 0.0125, 1.0f, 1.0f, 1.0f, 1.0f);
		drawText(texture, "Up: ^ ", -0.5f, 0.1f, 0.06f, 0.0125, 1.0f, 1.0f, 1.0f, 1.0f);
		drawText(texture, "Down: v ", -0.5f, 0.0f, 0.06f, 0.0125, 1.0f, 1.0f, 1.0f, 1.0f);
		drawText(texture, "PRESS ENTER TO START!", -1.1, -0.4, 0.1, 0.0125, 1.0, 0.0, 0.0, 1.0);
		break;
	case GAME_STATE:
		player.draw();
		for (size_t i = 0; i < asteroids.size(); i++) asteroids[i].draw();
		drawText(texture, "Time Left: " + to_string(timer), -1.2, 0.9, 0.07, 0.0, 1.0, 1.0, 1.0, 1.0);
		drawText(texture, "Lives: " + to_string(lives), 0.4, 0.9, 0.07, 0.0, 1.0, 1.0, 1.0, 1.0);
		break;
	case GAME_OVER:
		//lose game
		if (lives <= 0) {
			Mix_FreeMusic(backgroundMusic);
			Mix_PlayChannel(-1, lose, 0);
			drawText(texture, "YOU LOST!", -0.85f, 0.2f, 0.2f, 0.0, 1.0f, 0.0f, 0.0f, 1.0f);
			drawText(texture, "PRESS ESC TO EXIT", -0.85f, -0.4f, 0.1f, 0.0, 1.0f, 1.0f, 1.0f, 1.0f);
			break;
		}
		//win game
		else {
			Mix_FreeMusic(backgroundMusic);
			Mix_PlayChannel(-1, applause, 0);
			drawText(texture, "YOU WON!", -0.85f, 0.2f, 0.2f, 0.0, 1.0f, 0.0f, 0.0f, 1.0f);
			drawText(texture, "PRESS ESC TO EXIT", -0.85f, -0.4f, 0.1f, 0.0, 1.0f, 1.0f, 1.0f, 1.0f);
			break;
		}
	}
	SDL_GL_SwapWindow(displayWindow);
}
