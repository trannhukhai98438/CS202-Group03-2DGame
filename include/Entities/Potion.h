#pragma once
#include "Entities/Projectile.h"
#include "Utilities/Animator.h"
#include <SFML/Graphics.hpp>

class Potion : public Projectile {
private:
    sf::Sprite sprite;
    sf::Texture texture;
    Animator animator;
    bool isPuddle;
    float puddleTimer;
    const float gravity = 800.0f; // px/s^2
    const float groundY = 600.0f;

    void shatter();

public:
    Potion(float startX, float startY, float velX, float velY);
    ~Potion() override = default;

    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    
    // Override onHitPlayer to prevent hitting while flying if desired, or keep default
    void onHitPlayer(Character* player) override;
};
