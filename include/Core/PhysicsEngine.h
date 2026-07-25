#pragma once
#include <SFML/Graphics.hpp>

class PhysicsEngine {
private:
	const float GRAVITY = 980.f;
public:
	PhysicsEngine() = default;

	void applyGravity(float& velocityY, float dt);
	void resolveCollisionX(sf::RectangleShape& entity, const sf::RectangleShape& obstacle, float& velocityX);
	void resolveCollisionY(sf::RectangleShape& entity, const sf::RectangleShape& obstacle, float& velocityY);
};