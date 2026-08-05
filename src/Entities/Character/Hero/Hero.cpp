#include "Hero.h"
#include "SmallForm.h"
#include "IdleState.h"
#include "DeadState.h"
#include "Item.h"

Hero::Hero(float x, float y)
    : animator(sprite),
      invincibleTimer(0.f),
      overrideTimer(0.f),
      isStarman(false),
      coin(0),
      position({x, y}), isActive(true), isGrounded(false), isFacingRight(true),hp(1)
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
    // Flip sprite horizontally when facing left (scaled 2.0x to match enemy dimensions)
    if (isFacingRight) {
        sprite.setScale(2.f, 2.f);
    } else {
        sprite.setScale(-2.f, 2.f);
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

bool Hero::isDead(){
    return !isActive;
}

int Hero::getHp(){
    return hp;
}

sf::FloatRect Hero::getBounds(){
    return hitbox.getGlobalBounds();
}

void Hero::setSize(float x, float y){
    hitbox.setSize({x, y});
}

void Hero::setVelocity(float x, float y){
    velocity.x = x;
    velocity.y = y;
}

sf::Vector2f Hero::getVelocity(){
    return velocity;
}

bool Hero::getGrounded(){
    return isGrounded;
}

void Hero::setGrounded(bool grounded){
    isGrounded = grounded;
}

void Hero::setPosition(float x, float y){
    position.x = x;
    position.y = y;
    sf::Vector2f size = hitbox.getSize();
    hitbox.setPosition(position);
    sprite.setPosition(position.x + size.x / 2.f, position.y + size.y);
}

sf::Vector2f Hero::getPosition(){
    return position;
}

void Hero::setFacingRight(bool facing){
    isFacingRight = facing;
}

bool Hero::getFacingRight() const {
    return isFacingRight;
}

sf::RectangleShape& Hero::getHitbox(){
    return hitbox;
}