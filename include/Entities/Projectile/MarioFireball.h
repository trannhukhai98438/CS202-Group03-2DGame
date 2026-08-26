#pragma once

#include "Entities/Character/Enemy/Projectile.h"
#include "Utilities/Animator.h"

class MarioFireball : public Projectile {
private:
    sf::Sprite sprite;
    sf::Texture texture;
    Animator animator;
    float lifetime;

public:
    MarioFireball(float startX, float startY, float velocityX);

    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    float getGravityAcceleration() const override { return 980.0f; }
    void onSolidCollision(SideType side, const sf::FloatRect& solidBounds) override;
    void setPosition(const sf::Vector2f& pos) override;
};
