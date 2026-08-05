#pragma once
#include "Block.h"
#include <cmath>

class QuestionBlock : public Block {
public:
    QuestionBlock(float x, float y);
    ~QuestionBlock() = default;

    void update(float deltatime) override;
    void render(sf::RenderWindow& window) override;
    std::unique_ptr<Item> hit(Hero* hero) override;

private:
    float bounceTimer; // 0.1s countdown for visual bounce on hit
    float originalY;   // restore Y after bounce
};
