#include "Character.h"

Character::Character(float x, float y) : animator(sprite) {
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
}
