#pragma once
#include "Projectile.h"
#include "Utilities/Animator.h"
#include <SFML/Graphics.hpp>

class Potion : public Projectile {
private:
    sf::Sprite sprite;
    sf::Texture texture;
    sf::Texture puddleTexture;
    Animator animator;
    bool isPuddle;
    float puddleTimer;
    const float gravity = 1400.0f; // px/s^2
    void shatterAtImpact(SideType side, const sf::FloatRect& solidBounds);
    void shatterOnTile(float tileY);

public:
    Potion(float startX, float startY, float velX, float velY);
    ~Potion() override = default;

    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    std::string getProjectileType() const override { return "Potion"; }
    float getGravityAcceleration() const override { return isPuddle ? 0.0f : gravity; }
    bool usesWorldPhysics() const override { return !isPuddle; }
    void onSolidCollision(SideType side, const sf::FloatRect& solidBounds) override;
    void setPosition(const sf::Vector2f& pos) override;
};
