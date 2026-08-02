#include "Entities/BaseEntity.h"

BaseEntity::BaseEntity(float x, float y)
    : position(x, y),
      velocity(0.f, 0.f),
      animator(sprite),
      isAlive(true),
      facingRight(true),
      isGrounded(true) {
    shape.setPosition(position);
    shape.setSize(sf::Vector2f(32.f, 32.f));
    sprite.setPosition(position);
}

void BaseEntity::die() {
    isAlive = false;
}

void BaseEntity::takeDamage(int damage) {
    (void)damage;
    die();
}

sf::Vector2f BaseEntity::getPosition() const {
    return position;
}

void BaseEntity::setPosition(const sf::Vector2f& pos) {
    position = pos;
    shape.setPosition(position);
    sprite.setPosition(position);
}

sf::FloatRect BaseEntity::getBounds() const {
    sf::FloatRect bounds = shape.getGlobalBounds();
    bounds.left += 4.0f;
    bounds.width = std::max(1.0f, bounds.width - 8.0f);
    return bounds;
}

bool BaseEntity::getIsAlive() const {
    return isAlive;
}

bool BaseEntity::isDead() const {
    return !isAlive;
}
