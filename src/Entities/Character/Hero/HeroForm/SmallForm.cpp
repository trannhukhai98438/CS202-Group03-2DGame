#include "Hero.h"
#include "SmallForm.h"

void SmallForm::enter(Hero* hero){
    hero->setSize(16,16);
    hero->loadTexture(hero->getBaseTexturePath());
}

void SmallForm::update(Hero* hero, float deltatime){
    //nothing?
}

std::string SmallForm::getForm(){
    return "Small";
}