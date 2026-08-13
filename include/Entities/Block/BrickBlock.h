#pragma once
#include "Block.h"
#include "BrickParticle.h"
#include <array>
#include <cmath>

class BrickBlock : public Block {
public:
    BrickBlock(float x, float y);
    ~BrickBlock() = default;

    void update(float deltatime) override;
    void render(sf::RenderWindow& window) override;
    std::unique_ptr<Item> hit(Hero* hero) override;

    // Returns false while block is alive or particles are still showing.
    // Scene removes block from world once this returns false (isActive=false in base).
    bool getIsActive() const override { return isActive; }

private:
    bool  isDestroyed;   // true when Giant/Fire has destroyed the brick

    // --- Particles (Giant/Fire destruction) ---
    std::array<BrickParticle, 4> particles;
    bool  hasParticles;  // true while particles are alive
};
