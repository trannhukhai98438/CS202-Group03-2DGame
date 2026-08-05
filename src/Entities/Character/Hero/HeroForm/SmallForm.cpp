#include "SmallForm.h"
#include "Hero.h"

void SmallForm::enter(Hero* hero){
    sf::Vector2f oldsize=hero->getHitbox().getSize();
    hero->setSize(16, 16);
    hero->loadTexture(hero->getBaseTexturePath());
    sf::Vector2f oldposition=hero->getPosition();
    hero->setPosition(oldposition.x, oldposition.y + oldsize.y - 16.f);
}

void SmallForm::update(Hero* hero, float deltatime) {
}

std::string SmallForm::getForm(){
    return "Small";
}

void SmallForm::takedamage(Hero* hero) {
    hero->die();
}