<<<<<<< HEAD
#include "Entities/Character.h"

Character::Character(float x, float y)
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
    sprite.setPosition(position);
}

sf::FloatRect Character::getBounds() const {
    return shape.getGlobalBounds();
}

bool Character::getIsAlive() const {
    return isAlive;
}

bool Character::isDead() const {
    return !isAlive;
=======
#include "../../include/Entities/Character.h"

Character::Character(float x, float y){
    position={x,y};
    velocity={0.f, 0.f};
    facingRight=true;
    isGrounded=false;
    isActive=true;
}

void Character::die()
{
    isActive=false;
}

sf::FloatRect Character::getBounds(){
    return hitbox.getGlobalBounds();
}

bool Character::isDead() const{
    return (!isActive);
>>>>>>> 40981ce (fix: Change from hp logic (int) to state logic)
}
