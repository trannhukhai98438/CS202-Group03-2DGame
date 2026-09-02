#include "Hero.h"
#include "ShrinkState.h"
#include "IdleState.h"
#include "SmallForm.h"

ShrinkState::ShrinkState() : timer(0.f), maxTime(0.6f) {} // 4 frames × 0.15s = 0.6s

void ShrinkState::enter(Hero* hero){
    timer = 0.f;                        // reset in case of rapid re-hit
    hero->setVelocity(0.f, 0.f);        // lock movement during animation
    hero->setInvincible(2.0f, false);   // Damage I-frames start immediately
}

void ShrinkState::update(Hero* hero, float deltatime){
    timer += deltatime;
    if (timer >= maxTime){
        // Animation finished — apply the actual form change
        hero->setForm(std::make_unique<SmallForm>());
        hero->setState(std::make_unique<IdleState>());
    }
}

std::string ShrinkState::getState(){
    return "Shrink";
    // Animation key = formName + "Shrink"
    // Giant + Shrink = "GiantShrink" ✓
}
