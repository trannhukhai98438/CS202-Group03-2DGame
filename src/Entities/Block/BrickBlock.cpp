#include "Entities/Block/BrickBlock.h"
#include "Entities/Character/Hero/Hero.h"
#include <cmath>

BrickBlock::BrickBlock(float x, float y)
    : Block(x, y),
      isDestroyed(false),
      bounceTimer(0.f), originalY(y),
      hasParticles(false)
{
    if (texture.loadFromFile("assets/textures/Brick.png")) {
        sprite.setTexture(texture);
    }

    std::vector<sf::IntRect> brickFrames;
    brickFrames.push_back(sf::IntRect(1, 1, 16, 16));
    animator.addAnimation("Brick", Animation(brickFrames, 1.0f));

    // PieceofBrick animation — 4 frames cycled by animator;
    // rendered at 4 different particle positions simultaneously in render()
    animator.addAnimation("PieceofBrick", Animation({
        sf::IntRect(18, 1, 8, 8),
        sf::IntRect(18, 9, 8, 8),
        sf::IntRect(26, 1, 8, 8),
        sf::IntRect(26, 9, 8, 8)
    }, 0.2f));

    animator.playAnimation("Brick", 0.f);
}

// ---------------------------------------------------------------
void BrickBlock::update(float deltatime) {
    // --- Particle phase (Giant/Fire destruction) ---
    if (hasParticles) {
        // Advance shared animation frame for all particles
        animator.playAnimation("PieceofBrick", deltatime);

        bool allDone = true;
        for (auto& p : particles) {
            if (p.lifetime > 0.f) {
                p.vel.y    += 800.f * deltatime;  // gravity //temporary
                p.pos      += p.vel * deltatime;
                p.lifetime -= deltatime;
                allDone = false;
            }
        }
        if (allDone) {
            hasParticles = false;
            isActive     = false; // signal scene to remove this block
        }
        return;
    }

    if (isDestroyed) return;

    // --- Bounce effect (SmallForm hit) ---
    if (bounceTimer > 0.f) {
        bounceTimer -= deltatime;
        // sin curve: 0 → peak → 0 over 0.1s, max 6px upward
        float t      = 1.f - (bounceTimer / 0.1f);           // 0→1
        float offset = std::sin(t * 3.14159f) * 6.f;         // 0→6→0 px
        
        position.y   = originalY - offset;
        hitbox.setPosition(position);
        sprite.setPosition(position.x + 16.f, position.y + 32.f);
    } else {
        position.y = originalY;
        hitbox.setPosition(position);
        sprite.setPosition(position.x + 16.f, position.y + 32.f);
    }

    animator.playAnimation("Brick", deltatime);
}

// ---------------------------------------------------------------
void BrickBlock::render(sf::RenderWindow& window) {
    if (isDestroyed) {
        // Draw each living particle at its own physics position.
        // The animator already set the shared 8x8 texture rect via playAnimation.
        if (hasParticles) {
            for (const auto& p : particles) {
                if (p.lifetime > 0.f) {
                    sprite.setPosition(p.pos);
                    window.draw(sprite);
                }
            }
        }
        return;
    }
    window.draw(sprite);
}

// ---------------------------------------------------------------
std::unique_ptr<Item> BrickBlock::hit(Hero* hero) {
    if (isDestroyed) return nullptr;

    std::string form = hero->getFormName();
    if (form == "Giant" || form == "Fire") {
        // --- Destroy brick: launch 4 particles in different directions ---
        isDestroyed = true;
        hasParticles = true;

        float cx = position.x;
        float cy = position.y;

        // Each particle: position offset from center, unique direction
        // TL = top-left piece, TR = top-right, BL = bottom-left, BR = bottom-right
        // Frame driven by animator — no IntRect per particle
        particles[0] = {{ cx - 4.f, cy - 8.f }, { -80.f, -200.f }, 0.6f }; // TL
        particles[1] = {{ cx + 4.f, cy - 8.f }, {  80.f, -200.f }, 0.6f }; // TR
        particles[2] = {{ cx - 4.f, cy       }, { -80.f, -120.f }, 0.6f }; // BL
        particles[3] = {{ cx + 4.f, cy       }, {  80.f, -120.f }, 0.6f }; // BR

    } else {
        // --- SmallForm: visual bounce only, no break ---
        bounceTimer = 0.1f;
        originalY   = position.y;
    }

    return nullptr; // BrickBlock never spawns items
}
