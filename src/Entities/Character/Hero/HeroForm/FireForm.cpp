#include "Hero.h"
#include "FireForm.h"
#include "SitState.h"

void FireForm::enter(Hero* hero){
    hero->setSize(16, 32);
    hero->loadTexture("assets/textures/FireMario.png");
}

void FireForm::update(Hero* hero, float deltatime){
    std::string s = hero->getStateName();
    
    // Check Sit
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
        if (s == "Idle" || s == "Run"){
            hero->setState(std::make_unique<SitState>());
            return;
        }
    }

    // Check Special Ability
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)){
        if (s == "Idle" || s == "Run" || s == "Jump"){ 
            hero->specialAbility();
            hero->playOverrideAnimation("FireSpecial", 0.3f);
            return;
        }
    }
}

std::string FireForm::getForm(){
    return "Fire";
}

void FireForm::specialAbility(Hero* hero){
    // Spawn fireball entity here
}