#include "../../include/Entities/Mario.h"

Mario::Mario(float x, float y): Character(x,y){
    hp=1;
    isUntargetable=false;
    //initial setup for velocity, sprite and texture
}

void Mario::update(float deltatime){
    // later implementation

}

void Mario::render(sf::RenderWindow& window){
    window.draw(sprite);
}

void Mario::takedamage(int x){
    if (isUntargetable) return;
    hp-=x;
    if (hp<0) hp=0;
}