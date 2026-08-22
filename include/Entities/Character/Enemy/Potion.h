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

public:
    Potion(float startX, float startY, float velX, float velY);
    ~Potion() override = default;

    void shatterOnTile(float tileY);

    void setPosition(const sf::Vector2f& pos) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    
    // Override onHitPlayer to prevent hitting while flying if desired, or keep default
    void onHitPlayer(Character* player) override;

    bool getIsPuddle() const { return isPuddle; }
};
