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

	// --- RectangleShape overloads (Block/mapColliders vs Entity) ---
	SideType checkCollision(sf::RectangleShape& entity, const sf::RectangleShape& obstacle);
	void resolveCollisionX(sf::RectangleShape& entity, const sf::RectangleShape& obstacle, float& velocityX);
	void resolveCollisionY(sf::RectangleShape& entity, const sf::RectangleShape& obstacle, float& velocityY);

	// --- FloatRect overloads (synthesized tile bounds, future use) ---
	// Entity is still RectangleShape& so setPosition() works; obstacle is a
	// FloatRect computed from tile grid coords: {tileX*S, tileY*S, S, S}.
	SideType checkCollision(const sf::FloatRect& entityBounds, const sf::FloatRect& obstacleBounds);
	void resolveCollisionX(sf::RectangleShape& entity, const sf::FloatRect& obstacleBounds, float& velocityX);
	void resolveCollisionY(sf::RectangleShape& entity, const sf::FloatRect& obstacleBounds, float& velocityY);
};