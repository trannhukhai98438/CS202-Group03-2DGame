#pragma once
#include "Core/State.h"
#include <SFML/Graphics.hpp>
#include <memory>

class HUDManager;

class TransitionState : public State {
private:
	std::shared_ptr<HUDManager> m_hudManager;
	sf::Font m_font;
	sf::Text m_worldText;
	sf::Text m_livesText;
	sf::Time m_elapsedTime; //Track how long the transition has been displayed
public:
	TransitionState();
	explicit TransitionState(std::shared_ptr<HUDManager> hudManager);
	void processEvents(sf::Event& event) override;
	void update(sf::Time dt) override;
	void render(sf::RenderWindow& window) override;
};
