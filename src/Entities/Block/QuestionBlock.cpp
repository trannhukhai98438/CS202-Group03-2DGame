#include "QuestionBlock.h"
#include <cmath>

QuestionBlock::QuestionBlock(float x, float y)
    : Block(x, y), bounceTimer(0.f), originalY(y)
{
    if (texture.loadFromFile("assets/textures/QuestionBrick.png")) {
        sprite.setTexture(texture);
    }

    std::vector<sf::IntRect> questionFrames;
    for (int i = 0; i < 3; ++i) {
        questionFrames.push_back(sf::IntRect(i * 17 + 1, 1, 16, 16));
    }
    animator.addAnimation("QuestionBrick", Animation(questionFrames, 0.2f));

    std::vector<sf::IntRect> emptyFrames;
    emptyFrames.push_back(sf::IntRect(52, 1, 16, 16));
    animator.addAnimation("EmptyBrick", Animation(emptyFrames, 1.0f));

    animator.playAnimation("QuestionBrick", 0.f);
}

void QuestionBlock::update(float deltatime) {
    // --- Bounce effect (triggered by hit()) ---
    if (bounceTimer > 0.f) {
        bounceTimer -= deltatime;
        float t      = 1.f - (bounceTimer / 0.1f);     // 0→1
        float offset = std::sin(t * 3.14159f) * 6.f;   // 0→6→0 px
        
        position.y   = originalY - offset;
        hitbox.setPosition(position);
        sprite.setPosition(position.x + 16.f, position.y + 32.f);
    } else {
        position.y = originalY;
        hitbox.setPosition(position);
        sprite.setPosition(position.x + 16.f, position.y + 32.f);
    }

    // Animation: shimmer when active, static when spent
    if (isHit) animator.playAnimation("EmptyBrick",    deltatime);
    else        animator.playAnimation("QuestionBrick", deltatime);
}

void QuestionBlock::render(sf::RenderWindow& window) {
    window.draw(sprite);
}

std::unique_ptr<Item> QuestionBlock::hit(Hero* hero) {
    if (isHit) return nullptr; // Act as empty block, do nothing (no bounce, no spawn)

    isHit       = true;
    bounceTimer = 0.1f;    // trigger visual bounce
    originalY   = position.y;

    if (hiddenItemPrototype) {
        std::unique_ptr<Item> spawnedItem = hiddenItemPrototype->clone(hero);
        spawnedItem->spawn();
        return spawnedItem;
    }

    return nullptr;
}
