#pragma once
#include <SFML/Graphics.hpp>

enum class SideType{
	None,
	Top,
	Bottom,
	Left,
	Right
};

class PhysicsEngine {
private:
	const float GRAVITY = 980.f;
public:
	PhysicsEngine() = default;

	void applyGravity(float& velocityY, float dt);
	SideType checkCollision(sf::RectangleShape& entity, const sf::RectangleShape& obstacle);
	void resolveCollisionX(sf::RectangleShape& entity, const sf::RectangleShape& obstacle, float& velocityX);
	void resolveCollisionY(sf::RectangleShape& entity, const sf::RectangleShape& obstacle, float& velocityY);
};