#pragma once
#include "Item.h"


// this class is for pretending as a Mushroom and a Flower
class PowerUpPrototype : public Item {
public:
    PowerUpPrototype(float x, float y);
    ~PowerUpPrototype() = default;

    void update(float deltatime) override {}
    void render(sf::RenderWindow& window) override {}
    void spawn() override {}
    std::unique_ptr<Item> clone(Hero* hero) const override;
    void getCollected(Hero* hero) override {}
};
