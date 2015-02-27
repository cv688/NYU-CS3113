#include "SIGame.h"

int main(int argc, char *argv[]) {
	SpaceInvaders si;

	while (si.ProcessEvents()) {
		si.Update();
		si.Render();
	}
	return 0;
}