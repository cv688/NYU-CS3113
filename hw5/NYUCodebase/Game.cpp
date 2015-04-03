#include "Game.h"
#define TILE_MAP "tilemap.txt"
#define FIXED_TIMESTEP 0.0166666
#define MAX_TIMESTEPS 6

Game::Game() {
	Init();
}

void Game::Init() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Hw 5: Scrolling Platformer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 900, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	glViewport(0, 0, 900, 600);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0, 0.0, 0.0, 0.0);

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	backgroundMusic = Mix_LoadMUS("Kalimba.mp3");
	//repeat until game is stopped 
	Mix_PlayMusic(backgroundMusic, -1); 

	//sound effects
	pickup = Mix_LoadWAV("pickup.wav");
	unlock = Mix_LoadWAV("unlock.wav");

	state = MAIN_MENU;

	keys = SDL_GetKeyboardState(NULL);
	texture = loadTexture("texture.png");
	sheet = loadTexture("sprite.png");
	lastFrameTicks = 0.0;
	timeLeftOver = 0.0;
	player1Score = 0;
	player2Score = 0;
	playerWin = 0;
	
	//read tiles and entities from tile map
	ReadFile();

}

Game::~Game() {
	Mix_FreeMusic(backgroundMusic);
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
	glBindTexture(GL_TEXTURE_2D, font);
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

	glLoadIdentity();
	glTranslatef(x, y, 0.0);
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

void Game::ReadFile() {
	ifstream infile(TILE_MAP);
	string line;
	while (getline(infile, line)) {
		if (line == "[header]" && !ReadHeader(infile)) {
			return;
		}
		else if (line == "[layer]") {
			ReadLayerData(infile);
		}
		else if (line == "[ObjectLayer]") {
			ReadEntityData(infile);
		}
	}
}

bool Game::ReadHeader(ifstream &stream) {
	string line;
	mapWidth = -1;
	mapHeight = -1;
	while (getline(stream, line)) {
		if (line == "") { break; }
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "width") {
			mapWidth = atoi(value.c_str());
		}
		else if (key == "height"){
			mapHeight = atoi(value.c_str());
		}
	}

	if (mapWidth == -1 || mapHeight == -1) {
		return false;
	}
	else {
		levelData = new unsigned char*[mapHeight];
		for (int i = 0; i < mapHeight; ++i) {
			levelData[i] = new unsigned char[mapWidth];
		}
		return true;
	}
}

bool Game::ReadLayerData(ifstream &stream) {
	string line;
	while (getline(stream, line)) {
		if (line == "") { break; }
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "data") {
			for (int y = 0; y < mapHeight; y++) {
				getline(stream, line);
				istringstream lineStream(line);
				string tile;
				for (int x = 0; x < mapWidth; x++) {
					getline(lineStream, tile, ',');
					unsigned char val = (unsigned char)atoi(tile.c_str());
					if (val > 0) {
						levelData[y][x] = val - 1;
					}
					else {
						levelData[y][x] = 12;
					}
				}
			}
		}
	}
	return true;
}

bool Game::ReadEntityData(ifstream &stream) {
	string line, type;
	while (getline(stream, line)) {
		if (line == "") { break; }
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "type") {
			type = value;
		}
		else if (key == "location") {
			istringstream lineStream(value);
			string xPosition, yPosition;
			getline(lineStream, xPosition, ',');
			getline(lineStream, yPosition, ',');
			float placeX = atoi(xPosition.c_str()) / 16 * 0.08f + 0.08f;
			float placeY = atoi(yPosition.c_str()) / 16 * -0.08f;
			placeEntity(type, placeX, placeY);
		}
	}
	return true;
}

void Game::placeEntity(string &type, const float &xCoord, const float &yCoord) {
	//create player1
	if (type == "player1") { 
		player1 = Entity(xCoord, yCoord, 80, 16, 8, 0.5, sheet);
	}
	//create player2
	if (type == "player2") { 
		player2 = Entity(xCoord, yCoord, 81, 16, 8, 0.5, sheet);

	}
	//create key
	if (type == "key") {
		keyX = xCoord;
		keyY = yCoord;
		key = Entity(xCoord, yCoord, 86, 16, 8, 0.5, sheet);
	}
	//create lock
	if (type == "lock") {
		lockX = xCoord;
		lockY = yCoord;
		lock = Entity(xCoord, yCoord, 7, 16, 8, 0.5, sheet);
	}
}


void Game::Reset() {
	// reset the flags and the button
	key = Entity(keyX, keyY, 86, 16, 8, 0.5, sheet);
	lock = Entity(lockX, lockY, 7, 16, 8, 0.5, sheet);
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

	//if either of the players have picked up the key, redraw it and set the flag
	if ((player1.entityCollision(key) || player2.entityCollision(key)) && getKey == 0) {
		Mix_PlayChannel(-1, pickup, 0);
		key = Entity(keyX, keyY, 86, 16, 8, 0.5, sheet);
		if (player1.entityCollision(key)) { getKey = 1; }
		if (player2.entityCollision(key)) { getKey = 2; }
	}

	//if either of the players have unlocked the door, redraw it and set the flag
	if ((key.entityCollision(lock) && getKey == 0)) {
		lock = Entity(lockX, lockY, 7, 16, 8, 0.5, sheet);
		if (player2.entityCollision(key)) { getKey = 1; }
		if (player2.entityCollision(key)) { getKey = 2; }
	}

	//update the positions of the key if they are picked up
	if (getKey == 1) {
		if (key.entityCollision(lock)) {
			key = Entity(keyX, keyY, 86, 16, 8, 0.5, sheet);
			lock = Entity(lockX, lockY, 7, 16, 8, 0.5, sheet);
			getKey = 0;
			Mix_PlayChannel(-1, unlock, 0);
			player1Score += 50;
		}
		else {
			key.x = (player1.x + (player1.width * player1.s * 0.5));
			key.y = player1.y;
		}
	}

	if (getKey == 2) {
		if (key.entityCollision(lock)) {
			key = Entity(keyX, keyY, 86, 16, 8, 0.5, sheet);
			lock = Entity(lockX, lockY, 7, 16, 8, 0.5, sheet);
			getKey = 0;
			Mix_PlayChannel(-1, unlock, 0);
			player2Score += 50;
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
	if (player1.collideBottom != true) player1.acceleration_y = -2.7;

	//player1 y-axis velocity and collision detection
	player1.velocity_y = lerp(player1.velocity_y, 0.0, FIXED_TIMESTEP * player1.friction_y);
	player1.velocity_y = (player1.velocity_y + (player1.acceleration_y * FIXED_TIMESTEP));
	player1.y = (player1.y + (player1.velocity_y * FIXED_TIMESTEP));

	//check if player1 has collided with the world in respect to the y-axis
	worldCollision("y", player1);

	//player1 x-axis velocity and collision detection
	player1.velocity_x = lerp(player1.velocity_x, 0.0, FIXED_TIMESTEP * player1.friction_x);
	player1.velocity_x = (player1.velocity_x + (player1.acceleration_x * FIXED_TIMESTEP));
	player1.x = (player1.x + (player1.velocity_x * FIXED_TIMESTEP));

	//check if player1 has collided with the world in respect to the x-axis
	worldCollision("x", player1);

	//player2 gravity
	if (player2.collideBottom != true) player2.acceleration_y = -2.7;

	//player2 y-axis velocity and collision detection
	player2.velocity_y = lerp(player2.velocity_y, 0.0, FIXED_TIMESTEP * player2.friction_y);
	player2.velocity_y = (player2.velocity_y + (player2.acceleration_y * FIXED_TIMESTEP));
	player2.y = (player2.y + (player2.velocity_y * FIXED_TIMESTEP));

	//check if player2 has collided with the world in respect to the y-axis
	worldCollision("y", player2);

	//player2 x-axis velocity and collision detection
	player2.velocity_x = lerp(player2.velocity_x, 0.0, FIXED_TIMESTEP * player2.friction_x);
	player2.velocity_x = (player2.velocity_x + (player2.acceleration_x * FIXED_TIMESTEP));
	player2.x = (player2.x + (player2.velocity_x * FIXED_TIMESTEP));

	//check if player2 has collided with the world in respect to the x-axis
	worldCollision("x", player2);

	//player1 wins
	if (player1Score >= 300) {
		playerWin = 1;
		state = GAME_OVER;
	}
	//player2 wins
	else if (player2Score >= 300) {
		playerWin = 2;
		state = GAME_OVER;
	}

}

//check if players collide with world objects
void Game::worldCollision(const char *axis, Entity &entity) {
	float buffer;
	if (strncmp(axis, "x", 1) == 0) {
		// check left
		buffer = tileCollision("x", entity.x - (entity.width * entity.s), entity.y);
		if (buffer != 0.0) {
			entity.x = entity.x - (buffer * FIXED_TIMESTEP * 0.08f);
			entity.velocity_x = 0.0;
			entity.collideLeft = true;
		}
		// check right
		buffer = tileCollision("x", entity.x + (entity.width * entity.s), entity.y);
		if (buffer != 0.0) {
			entity.x = entity.x + (buffer * FIXED_TIMESTEP * 0.08f);
			entity.velocity_x = 0.0;
			entity.collideRight = true;
		}
	}
	if (strncmp(axis, "y", 1) == 0) {
		// check bottom
		buffer = tileCollision("y", entity.x, entity.y - (entity.height * entity.s));
		if (buffer != 0.0) {
			entity.y = entity.y + buffer;
			entity.velocity_y = 0.0;
			entity.collideBottom = true;
		}
		// check top
		buffer = tileCollision("y", entity.x, entity.y + (entity.height * entity.s)); 
		if (buffer != 0.0) {
			entity.y = entity.y + (buffer - 0.08f);
			entity.velocity_y = 0.0;
			entity.collideTop = true;
		}
	}
}

float Game::tileCollision(const char *axis, const float &x, const float &y) {
	//x tile
	int gridX = (int)(x / 0.08f); 
	//y tile
	int gridY = (int)(-y / 0.08f);
	//check to see if it is outside the tile map
	if (gridX < 0 || gridX > mapWidth || gridY < 0 || gridY > mapHeight) {
		return 0.0;
	}
	if (strncmp(axis, "x", 1) == 0) {
		//1 corresponds to the id of the layer tiles and 32 to the id of the borders
		if (levelData[gridY][gridX] == 1 || levelData[gridY][gridX] == 32) {
			float xCoord = (gridX * 0.08f); 
			return -x - xCoord;
		}
		else return 0.0;
	}
	if (strncmp(axis, "y", 1) == 0) {
		if (levelData[gridY][gridX] == 1 || levelData[gridY][gridX] == 32) {
			float yCoord = (gridY * 0.08f); 
			return -y - yCoord;
		}
		else return 0.0;
	}
}

void Game::renderWorld() {

	//scrolling
	glLoadIdentity();
	translateX = (-player1.x - player2.x) * 0.475;
	translateY = (-player1.y - player2.y) * 0.475;
	glTranslatef(translateX, translateY, 0.0);

	//draw world
	renderTiles();

	//draw players
	player1.draw(translateX, translateY);
	player2.draw(translateX, translateY);

	//draw key
	key.draw(translateX, translateY);

	//draw lock
	lock.draw(translateX, translateY);

	glPushMatrix();
	glLoadIdentity();

	//draw player1 and player2 scores
	drawText(texture, to_string(player1Score), -0.85, 0.9, 0.09, 0.0125, 0.0, 1.0, 0.0, 1.0);
	drawText(texture, to_string(player2Score), 0.85, 0.9, 0.09, 0.0125, 1.0, 0.5, 0.0, 1.0);

	glPopMatrix();

}

void Game::renderTiles() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, sheet);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	vector<float> vertexData;
	vector<float> texCoordData;
	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			float u = (float)((int)levelData[y][x] % 16) / (float)16;
			float v = (float)((int)levelData[y][x] / 16) / (float)8;
			float spriteWidth = 1.0 / (float)16;
			float spriteHeight = 1.0 / (float)8;
			vertexData.insert(vertexData.end(), {0.08f * x, -0.08f * y, 0.08f * x, (-0.08f * y) - 0.08f, (0.08f * x) + 0.08f, 
				(-0.08f * y) - 0.08f, (0.08f * x) + 0.08f, -0.08f * y});
			texCoordData.insert(texCoordData.end(), {u, v, u, v + spriteHeight, u + spriteWidth, v + spriteHeight, u + spriteWidth, v});
		}
	}
	glVertexPointer(2, GL_FLOAT, 0, vertexData.data());
	glEnableClientState(GL_VERTEX_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, texCoordData.data());
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDrawArrays(GL_QUADS, 0, mapHeight * mapWidth * 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
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
		renderWorld();
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

