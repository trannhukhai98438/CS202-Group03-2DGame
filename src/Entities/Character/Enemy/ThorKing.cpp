#include "Entities/Character/Enemy/ThorKing.h"
#include "Entities/Character/Enemy/ThorKingState.h"
#include "Entities/Character/Enemy/Potion.h"
#include <iostream>

class BossFire : public Projectile {
private:
    sf::Texture tex;
    sf::Sprite fireSprite;
public:
    BossFire(float startX, float startY, float velX, float velY) 
        : Projectile(startX, startY, velX, velY, 2) {
        // Use a smaller 40x40 centered hitbox so it doesn't clip the floor
        shape.setSize(sf::Vector2f(40.f, 40.f));
        shape.setPosition(startX + 44.f, startY + 44.f);
        if (tex.loadFromFile("assets/textures/boss_fire.png")) {
            fireSprite.setTexture(tex);
        }
        float baseScaleX = 128.f / tex.getSize().x;
        float baseScaleY = 128.f / tex.getSize().y;
        fireSprite.setOrigin(tex.getSize().x / 2.0f, tex.getSize().y / 2.0f);
        if (velX > 0) {
            fireSprite.setScale(baseScaleX, baseScaleY);
        } else {
            fireSprite.setScale(-baseScaleX, baseScaleY);
        }
    }
    void update(float deltaTime) override {
        position += velocity * deltaTime;
        // Keep hitbox centered
        shape.setPosition(position.x + 44.f, position.y + 44.f);
        
        fireSprite.setPosition(position.x + 64.f, position.y + 64.f);
        if (fireSprite.getTexture()->getSize().x > 0) {
            float baseScaleX = 128.f / fireSprite.getTexture()->getSize().x;
            float baseScaleY = 128.f / fireSprite.getTexture()->getSize().y;
            
            if (velocity.x < 0) {
                // Moving left: since image naturally faces RIGHT, flip it!
                fireSprite.setScale(-baseScaleX, baseScaleY);
            } else {
                // Moving right: no flip
                fireSprite.setScale(baseScaleX, baseScaleY);
            }
        }
    }
    void render(sf::RenderWindow& window) override {
        if (tex.getSize().x > 0) {
            window.draw(fireSprite);
        } else {
            window.draw(shape);
        }
    }
    bool shouldDieOnSolid() const override { return true; }
};

ThorKing::ThorKing(float startX, float startY, std::function<void(std::unique_ptr<Projectile>)> spawnCallback, float patrolRange)
    : Enemy(startX, startY, 60.f, patrolRange), m_bossHp(3), m_rollSpeed(500.f), m_spawnCallback(spawnCallback) {
    shape.setSize(sf::Vector2f(96.f, 128.f));
    health = 3;
    speed = 60.f;
    
    // Load texture based on prompt
    loadSpriteTexture("assets/textures/boss.png", 1, 128.f, 1);
    
    animatorComp.initAnimations(*this);
    changeState(std::make_unique<TKPatrolState>());
}

void ThorKing::update(float deltaTime) {
    if (!isAlive) return;
    if (currentState) {
        currentState->update(*this, deltaTime);
    }
}

float ThorKing::getSpeed() const {
    if (getStateName() == "TKRolling") return m_rollSpeed;
    if (getStateName() == "TKPatrol" || getStateName() == "TKFire") return speed;
    return 0.f;
}

int ThorKing::getDamageOnTouch() const {
    if (getStateName() == "TKStunned") return 0;
    return 2;
}

void ThorKing::onStomped(Character* attacker) {
    physics.onStomped(*this, attacker);
}

void ThorKing::onSideCollision(Character* attacker) {
    physics.onSideCollision(*this, attacker);
}

void ThorKing::takeDamage(int damage) {
    physics.takeDamage(*this, damage);
}

void ThorKing::notifyWallHit() {
    physics.notifyWallHit(*this);
}

void ThorKing::spawnFireProjectile() {
    if (m_spawnCallback) {
        float dirX = facingRight ? 1.0f : -1.0f;
        // Spawn the fire projectile closer to the mouth and slightly higher
        float sX = position.x + shape.getSize().x / 2.f + dirX * 75.f;
        float sY = position.y + 35.f;
        m_spawnCallback(std::make_unique<BossFire>(sX, sY, dirX * 400.f, 0.f));
    }
}

void ThorKing::checkObstacles() {
    physics.checkObstacles(*this);
}

void ThorKing::move(float deltaTime) {
    physics.move(*this, deltaTime);
}

void ThorKing::applyAnimation() {
    animatorComp.applyAnimation(*this);
}

void ThorKing::render(sf::RenderWindow& window) {
    if (!isAlive) return;
    if (sprite.getTexture() != nullptr) {
        window.draw(sprite);
    } else {
        window.draw(shape);
    }
}
