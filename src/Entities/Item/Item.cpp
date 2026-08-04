#include "Item.h"

Item::Item(float x, float y)
    : position({x, y}), animator(sprite),
      velocity(0.f, 0.f), isActive(false), isGrounded(false)
{
    sprite.setPosition(position);
    hitbox.setPosition(position);
}

bool Item::isCollected() const{
    return !isActive;
}

sf::FloatRect Item::getBounds() const{
    return hitbox.getGlobalBounds();
}

bool Item::getIsGrounded() const{
    return isGrounded;
}

void Item::setGrounded(bool grounded){
    isGrounded=grounded;
}

sf::Vector2f Item::getPosition() const {
    return position;
}

void Item::setPosition(float x, float y) {
    position = {x, y};
    sprite.setPosition(position);
    hitbox.setPosition(position);
}

void Item::getCollision(){

}