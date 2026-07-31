#include "Hero.h"
#include "GiantForm.h"
#include "SitState.h"

void GiantForm::enter(Hero* hero){
    hero->setSize(16, 32);
    hero->loadTexture(hero->getBaseTexturePath());
}

void GiantForm::update(Hero* hero, float deltatime){
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
        std::string s = hero->getStateName();
        if (s == "Idle" || s == "Run"){
            hero->setState(std::make_unique<SitState>());
        }
    }
}

std::string GiantForm::getForm(){
    return "Giant";
}