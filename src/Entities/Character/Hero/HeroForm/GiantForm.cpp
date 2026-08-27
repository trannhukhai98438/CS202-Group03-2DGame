#include "Hero.h"
#include "GiantForm.h"
#include "ShrinkState.h"
#include "SitState.h"
#include "IdleState.h"

void GiantForm::enter(Hero* hero){
    sf::Vector2f oldsize=hero->getHitbox().getSize();
    hero->setSize(32.f, 64.f);
    hero->loadTexture(hero->getBaseTexturePath());
    sf::Vector2f oldposition=hero->getPosition();
    hero->setPosition(oldposition.x, oldposition.y + oldsize.y - 64.f);
}

void GiantForm::update(Hero* hero, float deltatime){
    // Allow sitting only when Idle or Run (which implies grounded)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
        std::string s = hero->getStateName();
        if (s == "Idle" || s == "Run"){
            hero->setState(std::make_unique<SitState>());
        }
    }
}

std::string GiantForm::getForm(){
    return "Giant";
}

void GiantForm::takedamage(Hero* hero) {
    // Giant hit → play Shrink animation → transition to SmallForm
    // ShrinkState::enter() handles: lock velocity, reset timer, setInvincible(2.0f)
    hero->setState(std::make_unique<ShrinkState>());
}
