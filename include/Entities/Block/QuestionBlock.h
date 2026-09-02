#pragma once
#include "Block.h"

class QuestionBlock : public Block {
private:
    static sf::Texture s_texture;
    static bool s_isTextureLoaded;

public:
    QuestionBlock(float x, float y);
    ~QuestionBlock() = default;

    void update(float deltatime) override;
    void render(sf::RenderWindow& window) override;
    std::unique_ptr<Item> hit(Hero* hero) override;
};
