#include "Core/Game.h"
#include "Core/MainMenuState.h"
#include <memory>

#if defined(_WIN32)
#include <stdio.h>
extern "C" {
    FILE* __cdecl __iob_func(void) {
        static FILE iob[] = { *stdin, *stdout, *stderr };
        return iob;
    }
    FILE* (__cdecl *__imp___iob_func)(void) = __iob_func;
}
#endif

int main() {
	Game::getInstance().pushState(std::make_unique<MainMenuState>());
	Game::getInstance().run();
	return 0;
}