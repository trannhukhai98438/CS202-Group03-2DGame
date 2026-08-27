#include "Hero.h"
#include "SmallForm.h"
#include "IdleState.h"
#include "DeadState.h"
#include "Item.h"

Hero::Hero(float x, float y, ProjectileSpawnCallback spawnCallback)
    : Character(x, y),
      invincibleTimer(0.f),
      overrideTimer(0.f),
      isStarman(false),
      coin(0),
      hp(1),
      spawnProjectileCallback(std::move(spawnCallback))
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

std::string Hero::getSpecialTexturePath() const {
	return specialTexturePath.empty() ? baseTexturePath : specialTexturePath;
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
    float renderScale = spriteRenderScale;
    if (specialSpriteRenderScale > 0.f && form
        && form->getForm() == "Fire") {
        renderScale = specialSpriteRenderScale;
    }

    // Flip sprite based on facing direction
    if (facingRight) {
		sprite.setScale(renderScale, renderScale);
    } else {
		sprite.setScale(-renderScale, renderScale);
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
    // An override belongs to the old form's texture coordinates and must not
    // survive a texture/form swap.
    overrideTimer = 0.f;
    overrideAnim.clear();
    form = std::move(newForm);
    if (form) form->enter(this);
    if (form && state) {
        animator.playAnimation(form->getForm() + state->getState(), 0.f);
    }
}

void Hero::setState(std::unique_ptr<HeroState> newState){
    if (state) state->exit(this);
    state = std::move(newState);
    if (state) state->enter(this);
    if (form && state && overrideTimer <= 0.f) {
        animator.playAnimation(form->getForm() + state->getState(), 0.f);
    }
}

bool Hero::specialAbility(){
    if (!spawnProjectileCallback) return false;
    std::unique_ptr<Projectile> projectile = createSpecialProjectile();
    if (!projectile) return false;
    spawnProjectileCallback(std::move(projectile));
    return true;
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

void Hero::takeDamage(int damage){
    if (getStateName() == "Cheer") {
        return;
    }
    if (invincibleTimer > 0.f) {
        return; // Ignore damage during I-frames
    }
    if (form) {
        form->takedamage(this);
    }
}

void Hero::die(){
    if (!getIsAlive()) return;
    Character::die(); // sets isAlive = false
    setState(std::make_unique<DeadState>());
}

void Hero::collectItem(Item* item){
    if (item) item->getCollected(this);
}

bool Hero::isDead() const {
    return hp <= 0 || !isAlive;
}

int Hero::getHp(){
    return hp;
}

void Hero::setSize(float x, float y){
    shape.setSize({x, y});
}

void Hero::setPosition(float x, float y){
    position.x = x;
    position.y = y;
    sf::Vector2f size = shape.getSize();
    shape.setPosition(position);
    sprite.setPosition(position.x + size.x / 2.f, position.y + size.y);
}

int Hero::interactWith(Character* other) {
    if (!other || !other->getIsAlive()) return 0;
    
    sf::FloatRect enemyBounds = other->getHitbox().getGlobalBounds();
    sf::FloatRect heroBounds = getBounds();

    bool isFallingInAir = (!isGrounded && velocity.y >= 0.f);
    float marioBottomY = heroBounds.top + heroBounds.height;
    float enemyTopY = enemyBounds.top;

    // Stomp logic: Mario must be falling from the air and hit the top half of the enemy
    if (isFallingInAir && marioBottomY <= enemyTopY + (enemyBounds.height * 0.6f)) {
        other->onStomped(this);
        setVelocity(velocity.x, -300.f); // Bounce Hero up!
        return other->getScoreValue();   // Return score to be added
    } else {
        // The collided enemy owns the side-collision response. Its implementation
        // decides whether this contact damages the hero, kicks a shell, etc.
        // Calling takeDamage() here as well would apply the same hit twice.
        other->onSideCollision(this);
        return 0;
    }
}

void Hero::onStomped(Character* attacker){
    takeDamage(1);
}
void Hero::onSideCollision(Character* attacker){
    takeDamage(1);
}
