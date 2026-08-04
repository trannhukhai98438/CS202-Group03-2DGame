#pragma once
#include "Item.h"
#include "FireForm.h"

class Flower : public Item {
public:
    Flower(float x, float y);
    ~Flower() = default;

    void update(float deltatime) override;
    void render(sf::RenderWindow& window) override;
    void spawn() override;
    std::unique_ptr<Item> clone(Hero* hero) const override;
    void getCollected(Hero* hero) override;

private:
    bool isSpawning;
    float spawnStartY;
};
