#include "Character.h"

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
    sf::Vector2f size = hitbox.getSize();
    hitbox.setPosition(position);
    sprite.setPosition(position.x + size.x / 2.f, position.y + size.y);
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

sf::RectangleShape& Character::getHitbox(){
    return hitbox;
}