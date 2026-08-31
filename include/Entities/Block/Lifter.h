#pragma once

#include "Entities/Block/Block.h"

class Lifter final : public Block {
public:
    Lifter(float x, float y,
           float width, float height,
           float topBoundaryY, float bottomBoundaryY,
           bool movesUp);

    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    std::unique_ptr<Item> hit(Hero* hero) override;

    bool canBeHitFromBelow() const override { return false; }
    sf::Vector2f getVelocity() const override { return velocity; }

private:
    static constexpr float MOVEMENT_SPEED = 80.0f;

    sf::Vector2f velocity;
    float topBoundary;
    float bottomBoundary;
};
