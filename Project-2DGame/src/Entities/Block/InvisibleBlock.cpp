#include "Entities/Block/InvisibleBlock.h"

InvisibleBlock::InvisibleBlock(float x, float y)
    : QuestionBlock(x, y) {}

void InvisibleBlock::render(sf::RenderWindow& window) {
    if (isHit) {
        QuestionBlock::render(window);
    }
}
