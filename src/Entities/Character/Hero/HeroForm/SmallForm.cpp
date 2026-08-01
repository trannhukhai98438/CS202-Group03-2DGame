#include "SmallForm.h"
#include "Hero.h"

void SmallForm::enter(Hero* hero){
    hero->setSize(16,16);
    hero->loadTexture(hero->getBaseTexturePath());
}

void SmallForm::update(Hero* hero, float deltatime) {
}

std::string SmallForm::getForm(){
    return "Small";
}

void SmallForm::takedamage(Hero* hero) {
    hero->die();
}