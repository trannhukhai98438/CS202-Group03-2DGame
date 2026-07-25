#include "Entities/Character.h"

Character::Character(float x, float y)
    : position(x, y), velocity(0.f, 0.f), isAlive(true), facingRight(true), isGrounded(true) {
    shape.setPosition(position);
    shape.setSize(sf::Vector2f(32.f, 32.f));
}

void Character::die() {
    isAlive = false;
}

void Character::takeDamage(int damage) {
    (void)damage;
    die();
}

sf::Vector2f Character::getPosition() const {
    return position;
}

void Character::setPosition(const sf::Vector2f& pos) {
    position = pos;
    shape.setPosition(position);
}

sf::FloatRect Character::getBounds() const {
    return shape.getGlobalBounds();
}

bool Character::getIsAlive() const {
    return isAlive;
}

bool Character::isDead() const {
    return !isAlive;
}
