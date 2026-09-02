#pragma once

#include "Entities/Character/Enemy/Projectile.h"
#include "Utilities/Animator.h"

class MarioFireball : public Projectile {
private:
    sf::Sprite sprite;
    sf::Texture texture;
    Animator animator;
    float lifetime;
    float explosionTimer;
    bool isExploding;

    void enterExplosion();

public:
    MarioFireball(float startX, float startY, float velocityX);

    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    std::string getProjectileType() const override { return "MarioFireball"; }
    float getGravityAcceleration() const override { return 980.0f; }
    bool usesWorldPhysics() const override { return !isExploding; }
    void onSolidCollision(SideType side, const sf::FloatRect& solidBounds) override;
    bool onHitTarget(Character& target) override;
    void setPosition(const sf::Vector2f& pos) override;
};
