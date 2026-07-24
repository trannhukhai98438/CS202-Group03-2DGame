#include "Core/PhysicsEngine.h"

void PhysicsEngine::applyGravity(float& velocityY, float dt) {
	velocityY += GRAVITY * dt; // v = v0 + g * t with v0 = 0
}

void PhysicsEngine::resolveCollisionX(sf::RectangleShape& entity, const sf::RectangleShape& obstacle, float& velocityX) {
	sf::FloatRect entityBounds = entity.getGlobalBounds();
	sf::FloatRect obsBounds = obstacle.getGlobalBounds();
	if (entityBounds.intersects(obsBounds)) {
		if (velocityX > 0.f) { // Moving right
			entity.setPosition(obsBounds.left - entityBounds.width, entityBounds.top);
		}
		else if (velocityX < 0.f) { // Moving left
			entity.setPosition(obsBounds.left + obsBounds.width, entityBounds.top);
		}
		velocityX = 0.f;
	}
}

void PhysicsEngine::resolveCollisionY(sf::RectangleShape& entity, const sf::RectangleShape& obstacle, float& velocityY) {
	sf::FloatRect entityBounds = entity.getGlobalBounds();
	sf::FloatRect obsBounds = obstacle.getGlobalBounds();
	if (entityBounds.intersects(obsBounds)) {
		if (velocityY > 0.f) { // Falling down
			entity.setPosition(entityBounds.left, obsBounds.top - entityBounds.height);
		}
		else if (velocityY < 0.f) { // Jumping up
			entity.setPosition(entityBounds.left, obsBounds.top + obsBounds.height);
		}
		velocityY = 0.f;
	}
}