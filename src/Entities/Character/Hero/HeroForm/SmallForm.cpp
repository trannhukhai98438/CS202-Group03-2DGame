#include "SmallForm.h"
#include "Hero.h"

void SmallForm::enter(Hero* hero){
    sf::Vector2f oldsize=hero->getHitbox().getSize();
    hero->setSize(32.f, 32.f);
    hero->loadTexture(hero->getBaseTexturePath());
    sf::Vector2f oldposition=hero->getPosition();
    hero->setPosition(oldposition.x, oldposition.y + oldsize.y - 32.f);
}

void SmallForm::update(Hero* hero, float deltatime) {
}

std::string SmallForm::getForm(){
    return "Small";
}

void SmallForm::takedamage(Hero* hero) {
    hero->die();
}