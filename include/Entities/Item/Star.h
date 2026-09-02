#pragma once
#include "Item.h"

class Star : public Item {
public:
    Star(float x, float y);
    ~Star() = default;

    void update(float deltatime) override;
    void render(sf::RenderWindow& window) override;
    void spawn() override;
    std::unique_ptr<Item> clone(Hero* hero) const override;
    std::string getItemType() const override;
    void getCollected(Hero* hero) override;
    void getCollision() override;
    bool isColliable() override;
private:
    bool isSpawning;
    float spawnStartY;
};
