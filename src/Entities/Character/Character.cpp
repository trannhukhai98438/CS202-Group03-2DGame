<<<<<<< HEAD:src/Entities/Character.cpp
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
=======
#include "Character.h"
>>>>>>> 141c08e (refactor: Restructure Entities files):src/Entities/Character/Character.cpp

Character::Character(float x, float y): animator(sprite), position({x, y}), isActive(true), isGrounded(false), isFacingRight(true),hp(1){
}

bool Character::isDead(){
    return !isActive;
}

int Character::getHp(){
    return hp;
}

sf::FloatRect Character::getBounds(){
    return hitbox.getGlobalBounds();
}

void Character::setSize(float x, float y){
    hitbox.setSize({x, y});
    hitbox.setOrigin(x / 2.0f, y); // Bottom-Center origin
}

void Character::setVelocity(float x, float y){
    velocity.x = x;
    velocity.y = y;
}

sf::Vector2f Character::getVelocity(){
    return velocity;
}

bool Character::getGrounded(){
    return isGrounded;
}

void Character::setGrounded(bool grounded){
    isGrounded = grounded;
}

void Character::setPosition(float x, float y){
    position.x = x;
    position.y = y;
    sprite.setPosition(position);
    hitbox.setPosition(position);
}

sf::Vector2f Character::getPosition(){
    return position;
}

void Character::setFacingRight(bool facing){
    isFacingRight = facing;
}

bool Character::getFacingRight() const {
    return isFacingRight;
}