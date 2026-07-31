#include "Hero.h"
#include "GrowState.h"
#include "IdleState.h"
#include "GiantForm.h" // For form transition

GrowState::GrowState() : timer(0.f), maxTime(0.45f) {}

void GrowState::enter(Hero* hero){
    // Lock movement
    hero->setVelocity(0.f, 0.f); 
}

void GrowState::update(Hero* hero, float deltatime){
    timer += deltatime;
    if (timer >= maxTime){
        // Change form and state after animation finishes
        hero->setForm(std::make_unique<GiantForm>());
        hero->setState(std::make_unique<IdleState>());
    }
}

std::string GrowState::getState(){
    return "Grow";
}
