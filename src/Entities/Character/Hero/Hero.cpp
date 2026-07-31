#include "Hero.h"
#include "SmallForm.h"
#include "IdleState.h"

Hero::Hero(float x, float y): Character(x,y){
    // form and state init are usually done in concrete classes now (Mario/Luigi)
    // because they need baseTexturePath setup first.
    coin=0;
}

void Hero::loadTexture(const std::string& path){
    if (texture.loadFromFile(path)){
        sprite.setTexture(texture);
    }
}

std::string Hero::getBaseTexturePath() const {
    return baseTexturePath;
}

void Hero::playOverrideAnimation(const std::string& animName, float duration) {
    overrideAnim = animName;
    overrideTimer = duration;
}

void Hero::update(float deltatime){
    // Update sprite/hitbox position to current logical position
    sprite.setPosition(position);
    hitbox.setPosition(position);

    if(form) form->update(this, deltatime);
    if(state) state->update(this, deltatime);
    
    if (overrideTimer > 0) {
        overrideTimer -= deltatime;
        animator.playAnimation(overrideAnim, deltatime);
    } else {
        std::string currentaction = "";
        if(form && state) currentaction = form->getForm() + state->getState();
        if (currentaction != current && !currentaction.empty()){
            current = currentaction;
        }
        animator.playAnimation(current, deltatime);
    }
}

void Hero::render(sf::RenderWindow& window){
    if (isFacingRight) {
        sprite.setScale(1.f, 1.f);
    } else {
        sprite.setScale(-1.f, 1.f);
    }
    // window.draw(hitbox); // Un-comment for debugging
    window.draw(sprite);
}

void Hero::setForm(std::unique_ptr<HeroForm> newForm){
    form = std::move(newForm);
    if(form) form->enter(this);
}

void Hero::setState(std::unique_ptr<HeroState> newState){
    if (state) state->exit(this);
    state = std::move(newState);
    if (state) state->enter(this);
}

void Hero::specialAbility(){
    if(form) form->specialAbility(this);
}

std::string Hero::getStateName() const {
    if (state) return state->getState();
    return "";
}