#pragma once
#include "Item.h"
#include "GiantForm.h"

class Mushroom : public Item {
public:
    Mushroom(float x, float y);
    ~Mushroom() = default;

    void update(float deltatime) override;
    void render(sf::RenderWindow& window) override;
    void spawn() override;
    std::unique_ptr<Item> clone(Hero* hero) const override;
    void getCollected(Hero* hero) override;
    void getCollision() override;
    bool isColliable() override;
private:
    bool isSpawning;
    float spawnStartY;
};
