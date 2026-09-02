#pragma once

#include "Entities/Block/QuestionBlock.h"

class InvisibleBlock : public QuestionBlock {
public:
    InvisibleBlock(float x, float y);
    ~InvisibleBlock() override = default;

    void render(sf::RenderWindow& window) override;

    bool isSolid() const override { return isHit; }
    bool canBeHitFromBelow() const override { return !isHit; }
};
