#include "Entities/Block/BrickBlock.h"
#include "Entities/Character/Hero/Hero.h"
#include "Gameplay/BlockThemePalette.h"

BrickBlock::BrickBlock(float x, float y,
                       const BlockThemePalette& themePalette)
    : Block(x, y),
      themePalette(themePalette),
      boundTheme(MapTheme::Unspecified),
      hasBoundTheme(false),
      isDestroyed(false),
      hasParticles(false)
{
    syncThemeTexture();

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

void BrickBlock::syncThemeTexture() {
    const MapTheme activeTheme = themePalette.getActiveTheme();
    if (hasBoundTheme && activeTheme == boundTheme) return;

    if (const sf::Texture* themedTexture =
            themePalette.getTexture(BlockVisual::Brick)) {
        sprite.setTexture(*themedTexture);
    }

    boundTheme = activeTheme;
    hasBoundTheme = true;
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

    animator.playAnimation("Brick", deltatime);
}

// ---------------------------------------------------------------
void BrickBlock::render(sf::RenderWindow& window) {
    // Pipe travel is resolved after block updates, so synchronize here as
    // well to apply the destination theme in the same rendered frame.
    syncThemeTexture();

    if (isDestroyed) {
        if (hasParticles) {
            // Particle frames are 8x8 native; set origin to their centre (4,4)
            // so setPosition(p.pos) places the sprite correctly.
            sprite.setOrigin(4.f, 4.f);
            for (const auto& p : particles) {
                if (p.lifetime > 0.f) {
                    sprite.setPosition(p.pos);
                    window.draw(sprite);
                }
            }
            // Restore normal block origin for next frame (if block survives somehow)
            sprite.setOrigin(8.f, 16.f);
        }
        return;
    }
    window.draw(sprite);
}

// ---------------------------------------------------------------
std::unique_ptr<Item> BrickBlock::hit(Hero* hero) {
    if (isDestroyed || !hero) return nullptr;

    if (hasHiddenItems()) {
        return releaseHiddenItem(hero);
    }

    std::string form = hero->getFormName();
    if (form == "Giant" || form == "Fire") {
        // --- Destroy brick: launch 4 particles in different directions ---
        isDestroyed = true;
        hasParticles = true;

        hero->playSFX("break_block");

        // Block hitbox is 32x32; position = top-left corner.
        // Use centre of hitbox so the explosion is symmetric.
        float cx = position.x + 16.f;
        float cy = position.y + 16.f;

        // Particle frames are 8x8 native (16x16 at scale 2).
        // Spawn them offset by half their rendered size (8px) from centre.
        particles[0] = {{ cx - 8.f, cy - 8.f }, { -80.f, -220.f }, 0.6f }; // TL
        particles[1] = {{ cx,       cy - 8.f }, {  80.f, -220.f }, 0.6f }; // TR
        particles[2] = {{ cx - 8.f, cy       }, { -80.f, -100.f }, 0.6f }; // BL
        particles[3] = {{ cx,       cy       }, {  80.f, -100.f }, 0.6f }; // BR

    } else {
        // SmallForm: brick is indestructible, do nothing
    }

    return nullptr;
}
