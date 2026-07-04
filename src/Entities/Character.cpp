#include "../../include/Entities/Character.h"

Character::Character(float x, float y){
    position={x,y};

}

void Character::takedamage(int x) // if the character have hp
{
    hp-=x;
    if (hp<0) hp=0;
}
sf::FloatRect Character::getBounds(){
    return hitbox.getGlobalBounds();
}

bool Character::isDead() const{
    return (hp==0);
}
