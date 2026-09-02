#pragma once
#include "Item.h"

class Coin: public Item{
public:
    Coin(float x, float y);
    ~Coin()=default;

    void update(float deltatime) override;
    void render(sf::RenderWindow& window) override;

    void spawn() override;        // bounce-up animation (spawned from block)
    void spawnAsGroundCoin();      // activate in-place (floating coin from map)
    std::unique_ptr<Item> clone(Hero* hero) const override;
    std::string getItemType() const override;
    std::string getCollectionSfx() const override;

    void getCollected(Hero* hero) override;

    bool isColliable() override;

private:
    float bounceTimer;
    bool  m_alreadyCollected; // true nếu đã được auto-collect qua clone() → tránh double-count
};
