#include "Core/Game.h"
#include "Core/MainMenuState.h"
#include <memory>
int main() {
	Game::getInstance().pushState(std::make_unique<MainMenuState>());
	Game::getInstance().run();
	return 0;
}