#include "QuestionBlock.h"

QuestionBlock::QuestionBlock(float x, float y)
    : Block(x, y)
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
    if (isHit) animator.playAnimation("EmptyBrick",    deltatime);
    else        animator.playAnimation("QuestionBrick", deltatime);
}

void QuestionBlock::render(sf::RenderWindow& window) {
    window.draw(sprite);
}

std::unique_ptr<Item> QuestionBlock::hit(Hero* hero) {
    if (isHit) return nullptr;

    std::unique_ptr<Item> spawnedItem = releaseHiddenItem(hero);
    if (!hasHiddenItems()) {
        isHit = true;
        animator.playAnimation("EmptyBrick", 0.f);
    }

    return spawnedItem;
}
