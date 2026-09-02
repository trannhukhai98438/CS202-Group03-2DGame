#pragma once
#include <SFML/Graphics.hpp>

class Game;

// State Pattern: Each state (e.g., Menu, Playing, Paused) will inherit from this base class and implement its own behavior 
// for processing events, updating, and rendering.
class State {
public:
	virtual ~State() = default;
	virtual void processEvents(sf::Event& event) = 0;
	virtual void update(sf::Time dt) = 0;
	virtual void render(sf::RenderWindow& window) = 0;
	virtual bool rendersBelow() const { return false; }
};
