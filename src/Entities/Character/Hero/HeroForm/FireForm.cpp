#include "Hero.h"
#include "FireForm.h"
#include "GiantForm.h"
#include "SitState.h"

FireForm::FireForm(): cooldown(3.0f), counttime(0.f){
}

void FireForm::enter(Hero* hero){
    sf::Vector2f oldsize=hero->getHitbox().getSize();
    hero->setSize(32.f, 64.f);
    hero->loadTexture("assets/textures/FireMario.png");
    counttime = 0.f;
    sf::Vector2f oldposition=hero->getPosition();
    hero->setPosition(oldposition.x, oldposition.y + oldsize.y - 64.f);
}

void FireForm::update(Hero* hero, float deltatime){
    std::string s = hero->getStateName();
    counttime += deltatime;

    // Check Sit — only when grounded (Idle/Run implies grounded)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
        if (s == "Idle" || s == "Run"){
            hero->setState(std::make_unique<SitState>());
            return;
        }
    }

    // Check Special Ability (fireball) with cooldown
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::X) && counttime >= cooldown){
        if (s == "Idle" || s == "Run" || s == "Jump"){
            hero->specialAbility();
            hero->playOverrideAnimation("FireSpecial", 0.3f);
            counttime = 0.f;
            return;
        }
    }
}

std::string FireForm::getForm(){
    return "Fire";
}

void FireForm::takedamage(Hero* hero) {
    // Fire hit → downgrade to Giant, then immediately trigger Giant's damage
    // Giant::takedamage → setState(ShrinkState) → plays "GiantShrink" → SmallForm
    // No invincible set here — ShrinkState::enter() handles it
    hero->setForm(std::make_unique<GiantForm>());
    hero->takedamage();
}

void FireForm::specialAbility(Hero* hero){
    // TODO: Spawn Fireball entity in the scene
}