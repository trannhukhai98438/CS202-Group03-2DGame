#pragma once
#include <memory>
#include "Entities/Item/Item.h"

enum class ItemType {
    Coin,
    Star,
    PowerUpPrototype,
    None
};

class ItemFactory {
public:
    static std::unique_ptr<Item> createItem(ItemType type, float x, float y);
};
