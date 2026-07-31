#include "Hero.h"
#include "ShrinkState.h"
#include "IdleState.h"
#include "SmallForm.h" // For form transition

ShrinkState::ShrinkState() : timer(0.f), maxTime(0.6f) {} // 4 frames at 0.15s each = 0.6s

void ShrinkState::enter(Hero* hero){
    // Lock movement
    hero->setVelocity(0.f, 0.f);
    // Usually also grant temporary invincibility here
}

void ShrinkState::update(Hero* hero, float deltatime){
    timer += deltatime;
    if (timer >= maxTime){
        // Change form and state after animation finishes
        hero->setForm(std::make_unique<SmallForm>());
        hero->setState(std::make_unique<IdleState>());
    }
}

std::string ShrinkState::getState(){
    return "Shrink";
}
