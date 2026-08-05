#pragma once
#include "Core/State.h"
#include "Core/PhysicsEngine.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class Hero;
class Item;
class Block;

class PlayingState : public State {
private:
	sf::View m_camera;
	std::unique_ptr<Hero> hero;
	std::vector<std::unique_ptr<Item>> items;
	std::vector<std::unique_ptr<Block>> blocks;
	sf::RectangleShape m_dummyFloor;
	sf::RectangleShape m_dummyWall;
	PhysicsEngine m_physics;
public:
	PlayingState();
	~PlayingState() override;
	void processEvents(sf::Event& event) override;
	void update(sf::Time dt) override;
	void render(sf::RenderWindow& window) override;
};