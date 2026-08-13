#include "Core/PhysicsEngine.h"

void PhysicsEngine::applyGravity(float& velocityY, float dt) {
	velocityY += GRAVITY * dt;
}

// ============================================================
// FloatRect CORE — all AABB math lives here
// ============================================================

SideType PhysicsEngine::checkCollision(const sf::FloatRect& eb,
                                        const sf::FloatRect& ob) {
	if (!eb.intersects(ob)) return SideType::None;

	float cex = eb.left + eb.width  * 0.5f;
	float cey = eb.top  + eb.height * 0.5f;
	float cox  = ob.left + ob.width  * 0.5f;
	float coy  = ob.top  + ob.height * 0.5f;

	float dx = cex - cox;
	float dy = cey - coy;

	float overlapX = (eb.width  * 0.5f + ob.width  * 0.5f) - std::abs(dx);
	float overlapY = (eb.height * 0.5f + ob.height * 0.5f) - std::abs(dy);

	if (overlapX < overlapY)
		return dx < 0.f ? SideType::Left : SideType::Right;
	else
		return dy < 0.f ? SideType::Top  : SideType::Bottom;
}

void PhysicsEngine::resolveCollisionX(sf::RectangleShape& entity,
                                       const sf::FloatRect& ob,
                                       float& velocityX) {
	sf::FloatRect eb = entity.getGlobalBounds();
	SideType side = checkCollision(eb, ob);
	if (side == SideType::Left) {
		entity.setPosition(ob.left - eb.width, eb.top);
		velocityX = 0.f;
	} else if (side == SideType::Right) {
		entity.setPosition(ob.left + ob.width, eb.top);
		velocityX = 0.f;
	}
}

void PhysicsEngine::resolveCollisionY(sf::RectangleShape& entity,
                                       const sf::FloatRect& ob,
                                       float& velocityY) {
	sf::FloatRect eb = entity.getGlobalBounds();
	SideType side = checkCollision(eb, ob);
	if (side == SideType::Top) {
		entity.setPosition(eb.left, ob.top - eb.height);
		velocityY = 0.f;
	} else if (side == SideType::Bottom) {
		entity.setPosition(eb.left, ob.top + ob.height);
		velocityY = 0.f;
	}
}

// ============================================================
// RectangleShape overloads — delegate to FloatRect core
// ============================================================

SideType PhysicsEngine::checkCollision(sf::RectangleShape& entity,
                                        const sf::RectangleShape& obstacle) {
	return checkCollision(entity.getGlobalBounds(), obstacle.getGlobalBounds());
}

void PhysicsEngine::resolveCollisionX(sf::RectangleShape& entity,
                                       const sf::RectangleShape& obstacle,
                                       float& velocityX) {
	resolveCollisionX(entity, obstacle.getGlobalBounds(), velocityX);
}

void PhysicsEngine::resolveCollisionY(sf::RectangleShape& entity,
                                       const sf::RectangleShape& obstacle,
                                       float& velocityY) {
	resolveCollisionY(entity, obstacle.getGlobalBounds(), velocityY);
}
