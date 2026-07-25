#pragma once
#include <SFML/Graphics.hpp>

class Game {
private:
	Game();
	sf::RenderWindow m_window;

	const sf::Time TimePerFrame = sf::seconds(1.f / 60.f);

	void processEvents();
	void update(sf::Time dt);
	void render();

public:
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;

	static Game& getInstance() {
		static Game instance;
		return instance;
	}

	void run();
};