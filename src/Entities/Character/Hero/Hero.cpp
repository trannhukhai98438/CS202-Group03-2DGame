#include "Hero.h"
#include "SmallForm.h"
#include "IdleState.h"
#include "DeadState.h"
#include "Item.h"

Hero::Hero(float x, float y)
    : Character(x, y),
      invincibleTimer(0.f),
      overrideTimer(0.f),
      isStarman(false),
      coin(0)
{
    // form and state are initialised in concrete subclasses (Mario/Luigi)
    // because baseTexturePath must be set first.
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
    // Countdown timers
    if (invincibleTimer > 0.f) {
        invincibleTimer -= deltatime;
    }

    // Update sprite/hitbox to logical position
    setPosition(position.x,position.y);

    // Delegate to form (handles form-specific input: Sit, Fire special)
    if (form) form->update(this, deltatime);

    // Delegate to state (handles input, physics, state transitions)
    if (state) state->update(this, deltatime);

    // Play animation — key is composed as FormName + StateName
    // e.g. "Small" + "Idle" = "SmallIdle"
    if (overrideTimer > 0.f) {
        overrideTimer -= deltatime;
        animator.playAnimation(overrideAnim, deltatime);
    } else {
        if (form && state) {
            animator.playAnimation(form->getForm() + state->getState(), deltatime);
        }
    }
}

void Hero::render(sf::RenderWindow& window){
    // Flip sprite horizontally when facing left
    if (isFacingRight) {
        sprite.setScale(1.f, 1.f);
    } else {
        sprite.setScale(-1.f, 1.f);
    }

    // Invincibility visual effects
    if (invincibleTimer > 0.f) {
        if (isStarman) {
            // Rainbow flash for Starman
            sprite.setColor(sf::Color(rand() % 256, rand() % 256, rand() % 256));
        } else {
            // Blink for damage I-frames
            int alpha = (static_cast<int>(invincibleTimer * 15) % 2 == 0) ? 100 : 255;
            sprite.setColor(sf::Color(255, 255, 255, alpha));
        }
    } else {
        sprite.setColor(sf::Color::White);
        isStarman = false;
    }

    // window.draw(hitbox); // Un-comment for hitbox debugging
    window.draw(sprite);
}

void Hero::setForm(std::unique_ptr<HeroForm> newForm){
    form = std::move(newForm);
    if (form) form->enter(this);
}

void Hero::setState(std::unique_ptr<HeroState> newState){
    if (state) state->exit(this);
    state = std::move(newState);
    if (state) state->enter(this);
}

void Hero::specialAbility(){
    if (form) form->specialAbility(this);
}

std::string Hero::getStateName() const {
    if (state) return state->getState();
    return "";
}

std::string Hero::getFormName() const {
    if (form) return form->getForm();
    return "";
}

void Hero::setInvincible(float duration, bool starman) {
    invincibleTimer = duration;
    isStarman = starman;
}

int Hero::getCoin() const{
    return coin;
}

void Hero::collectCoin(){
    ++coin;
}

void Hero::takedamage(){
    if (invincibleTimer > 0.f) {
        return; // Ignore damage during I-frames
    }
    if (form) {
        form->takedamage(this);
    }
}

void Hero::die(){
    isActive = false;
    setState(std::make_unique<DeadState>());
}

void Hero::collectItem(Item* item){
    if (item) item->getCollected(this);
}