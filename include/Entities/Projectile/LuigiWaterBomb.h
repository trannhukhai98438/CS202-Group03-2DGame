#pragma once

#include "Entities/Character/Enemy/Projectile.h"

#include <unordered_set>

class LuigiWaterBomb : public Projectile {
private:
    enum class Phase {
        Flying,
        Splash
    };

    sf::Sprite bombSprite;
    sf::Sprite splashSprite;
    sf::Texture bombTexture;
    sf::Texture splashTexture;
    Phase phase;
    float lifetime;
    float impactEffectTimer;
    bool damageWindowOpen;
    std::unordered_set<const Character*> hitTargets;

    void enterSplash(SideType side, const sf::FloatRect* solidBounds = nullptr);
    void configureSprite(sf::Sprite& target, const sf::Texture& source,
                         float targetWidth, float targetHeight);

public:
    LuigiWaterBomb(float startX, float startY, float velocityX, float velocityY);

    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    std::string getProjectileType() const override { return "LuigiWaterBomb"; }
    float getGravityAcceleration() const override;
    bool usesWorldPhysics() const override;
    void onSolidCollision(SideType side, const sf::FloatRect& solidBounds) override;
    bool onHitTarget(Character& target) override;
    void onTargetResolutionComplete() override;
    void setPosition(const sf::Vector2f& pos) override;
};
