#pragma once
#include <SFML/Graphics.hpp>
#include <stack>
#include <memory>
#include "Core/State.h"
#include "Entities/Character/Hero/HeroFactory.h"

class Game {
private:
	Game();
	sf::RenderWindow m_window;

	const sf::Time TimePerFrame = sf::seconds(1.f / 60.f);
	std::stack<std::unique_ptr<State>> m_states; // State stack
	HeroType m_selectedHero{HeroType::Mario};
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
	void pushState(std::unique_ptr<State> state);
	void popState();
	void changeState(std::unique_ptr<State> state);
	void setSelectedHero(HeroType heroType) { m_selectedHero = heroType; }
	HeroType getSelectedHero() const { return m_selectedHero; }
};
