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

SideType PhysicsEngine::checkCollision(sf::RectangleShape& entity, const sf::RectangleShape& obstacle){
	sf::FloatRect entityBounds = entity.getGlobalBounds();
	sf::FloatRect obsBounds = obstacle.getGlobalBounds();
	if (!entityBounds.intersects(obsBounds))
		return SideType::None;
	// Tâm của hai AABB
    float centerEntityX = entityBounds.left + entityBounds.width * 0.5f;
    float centerEntityY = entityBounds.top + entityBounds.height * 0.5f;

    float centerObsX = obsBounds.left + obsBounds.width * 0.5f;
    float centerObsY = obsBounds.top + obsBounds.height * 0.5f;

    // Khoảng cách giữa hai tâm
    float dx = centerEntityX - centerObsX;
    float dy = centerEntityY - centerObsY;

    // Độ chồng lấn theo X
    float overlapX =
        (entityBounds.width * 0.5f + obsBounds.width * 0.5f) - std::abs(dx);

    // Độ chồng lấn theo Y
    float overlapY =
        (entityBounds.height * 0.5f + obsBounds.height * 0.5f) - std::abs(dy);



    // Trục có overlap nhỏ hơn là hướng va chạm
    if (overlapX < overlapY)
    {
        if (dx < 0)
            return SideType::Left;
        else
            return SideType::Right;
    }
    else
    {
        if (dy < 0)
            return SideType::Top;
        else
            return SideType::Bottom;
    }

    return SideType::None;
}