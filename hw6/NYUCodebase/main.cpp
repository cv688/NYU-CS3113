#include "Game.h"

int main(int argc, char *argv[]) {
	Game asteroids;
	while (asteroids.ProcessEvents()) {}
	return 0;
}