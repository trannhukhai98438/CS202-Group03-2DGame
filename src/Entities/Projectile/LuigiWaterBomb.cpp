#include "Entities/Projectile/LuigiWaterBomb.h"
#include "Entities/Character/Character.h"

LuigiWaterBomb::LuigiWaterBomb(float startX, float startY,
                               float velocityX, float velocityY)
    : Projectile(startX, startY, velocityX, velocityY,
                 ProjectileFaction::Hero, 1),
      phase(Phase::Flying), lifetime(4.0f), impactEffectTimer(0.18f),
      damageWindowOpen(false) {
    shape.setSize({26.0f, 26.0f});
    shape.setFillColor(sf::Color(50, 170, 255));

    if (bombTexture.loadFromFile("assets/textures/LuigiWaterBomb.png")) {
        bombTexture.setSmooth(true);
        bombSprite.setTexture(bombTexture);
        configureSprite(bombSprite, bombTexture, 32.0f, 32.0f);
    }
    if (splashTexture.loadFromFile("assets/textures/LuigiWaterSplash.png")) {
        splashTexture.setSmooth(true);
        splashSprite.setTexture(splashTexture);
        configureSprite(splashSprite, splashTexture, 110.0f, 38.0f);
    }

    setPosition({startX, startY});
}

void LuigiWaterBomb::configureSprite(sf::Sprite& target,
                                      const sf::Texture& source,
                                      float targetWidth, float targetHeight) {
    sf::Vector2u size = source.getSize();
    if (size.x == 0 || size.y == 0) return;
    target.setOrigin(size.x * 0.5f, static_cast<float>(size.y));
    target.setScale(targetWidth / static_cast<float>(size.x),
                    targetHeight / static_cast<float>(size.y));
}

void LuigiWaterBomb::update(float deltaTime) {
    if (!isAlive) return;

    velocity.y += getGravityAcceleration() * deltaTime;

    if (phase == Phase::Flying) {
        lifetime -= deltaTime;
        if (lifetime <= 0.0f) die();
    } else {
        impactEffectTimer -= deltaTime;
        if (impactEffectTimer <= 0.0f) die();
    }
}

void LuigiWaterBomb::render(sf::RenderWindow& window) {
    if (!isAlive) return;
    if (phase == Phase::Flying && bombSprite.getTexture()) {
        window.draw(bombSprite);
    } else if (phase == Phase::Splash && splashSprite.getTexture()) {
        window.draw(splashSprite);
    } else {
        window.draw(shape);
    }
}

float LuigiWaterBomb::getGravityAcceleration() const {
    return phase == Phase::Flying ? 1400.0f : 0.0f;
}

bool LuigiWaterBomb::usesWorldPhysics() const {
    return phase == Phase::Flying;
}

void LuigiWaterBomb::enterSplash(SideType side,
                                  const sf::FloatRect* solidBounds) {
    if (phase == Phase::Splash) return;

    sf::Vector2f center(position.x + shape.getSize().x * 0.5f,
                        position.y + shape.getSize().y * 0.5f);
    phase = Phase::Splash;
    impactEffectTimer = 0.18f;
    damageWindowOpen = true;
    velocity = {0.0f, 0.0f};
    shape.setSize({80.0f, 24.0f});

    sf::Vector2f splashPosition(center.x - 40.0f, center.y - 12.0f);
    if (side == SideType::Top && solidBounds) {
        splashPosition.y = solidBounds->top - shape.getSize().y;
    }
    setPosition(splashPosition);
}

void LuigiWaterBomb::onSolidCollision(SideType side,
                                       const sf::FloatRect& solidBounds) {
    enterSplash(side, &solidBounds);
}

bool LuigiWaterBomb::onHitTarget(Character& target) {
    if (!isAlive || !target.getIsAlive()) return false;
    if (phase == Phase::Splash && !damageWindowOpen) return false;
    if (hitTargets.find(&target) != hitTargets.end()) return false;

    if (phase == Phase::Flying) enterSplash(SideType::None);
    hitTargets.insert(&target);
    target.takeDamage(damage);
    return true;
}

void LuigiWaterBomb::onTargetResolutionComplete() {
    if (phase == Phase::Splash) damageWindowOpen = false;
}

void LuigiWaterBomb::setPosition(const sf::Vector2f& pos) {
    Projectile::setPosition(pos);
    sf::Vector2f bottomCenter(position.x + shape.getSize().x * 0.5f,
                              position.y + shape.getSize().y);
    bombSprite.setPosition(bottomCenter);
    splashSprite.setPosition(bottomCenter);
}
