#include "Item.h"
#include "Hero.h"

Item::Item(float x, float y)
    : position({x, y}), animator(sprite),
      velocity(0.f, 0.f), isActive(false), isGrounded(false)
{

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
    sf::Vector2f size = hitbox.getSize();
    hitbox.setPosition(position);
    sprite.setPosition(position.x + size.x / 2.f, position.y + size.y);
}

void Item::getCollision(){

}

sf::RectangleShape& Item::getHitbox(){
    return hitbox;
}

void Item::setVelocity(float x, float y){
    velocity={x,y};
}

sf::Vector2f Item::getVelocity(){
    return velocity;
}
