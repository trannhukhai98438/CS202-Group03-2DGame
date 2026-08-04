#include "ItemFactory.h"
#include "Entities/Item/Coin.h"
#include "Entities/Item/Star.h"
#include "Entities/Item/PowerUpPrototype.h"

std::unique_ptr<Item> ItemFactory::createItem(ItemType type, float x, float y) {
    switch (type) {
        case ItemType::Coin:
            return std::make_unique<Coin>(x, y);
        case ItemType::Star:
            return std::make_unique<Star>(x, y);
        case ItemType::PowerUpPrototype:
            return std::make_unique<PowerUpPrototype>(x, y);
        case ItemType::None:
        default:
            return nullptr;
    }
}
