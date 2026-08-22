#include "Entities/Character/Enemy/ThorKing.h"
#include "Entities/Character/Enemy/ThorKingState.h"

class BossFire : public Projectile {
private:
    sf::Texture tex;
    sf::Sprite fireSprite;

    void updateSpriteTransform() {
        if (tex.getSize().x == 0 || tex.getSize().y == 0) return;

        const float scaleX = 128.f / static_cast<float>(tex.getSize().x);
        const float scaleY = 128.f / static_cast<float>(tex.getSize().y);
        fireSprite.setScale(velocity.x < 0.f ? -scaleX : scaleX, scaleY);
        fireSprite.setPosition(position.x + shape.getSize().x * 0.5f,
                               position.y + shape.getSize().y * 0.5f);
    }

public:
    BossFire(float startX, float startY, float velX, float velY)
        : Projectile(startX, startY, velX, velY,
                     ProjectileFaction::Enemy, 2) {
        shape.setSize({40.f, 40.f});
        shape.setFillColor(sf::Color(255, 180, 40));
        shape.setPosition(position);

        if (tex.loadFromFile("assets/textures/boss_fire.png")) {
            fireSprite.setTexture(tex);
            fireSprite.setOrigin(tex.getSize().x * 0.5f,
                                 tex.getSize().y * 0.5f);
        }
        updateSpriteTransform();
    }

    void update(float) override {
        if (!isAlive) return;
        updateSpriteTransform();
    }

    void setPosition(const sf::Vector2f& pos) override {
        Projectile::setPosition(pos);
        updateSpriteTransform();
    }

    void onSolidCollision(SideType, const sf::FloatRect&) override {
        die();
    }

    void render(sf::RenderWindow& window) override {
        if (!isAlive) return;
        if (fireSprite.getTexture() != nullptr) {
            window.draw(fireSprite);
        } else {
            window.draw(shape);
        }
    }
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
