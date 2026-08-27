#include "Entities/Block/Block.h"

Block::Block(float x, float y) : position(x, y), isHit(false), isActive(true), animator(sprite) {
    hitbox.setSize({32.f, 32.f});
    sprite.setScale(2.f, 2.f);
    sprite.setOrigin(8.f, 16.f); // Bottom-center origin of the 16x16 base texture
    hitbox.setPosition(position);
    sprite.setPosition(position.x + 16.f, position.y + 32.f);
}

void Block::setItemPrototype(std::unique_ptr<Item> itemProto, int itemCount) {
    hiddenItemPrototype = std::move(itemProto);
    hiddenItemCount = hiddenItemPrototype && itemCount > 0 ? itemCount : 0;
    if (hiddenItemCount == 0) {
        hiddenItemPrototype.reset();
    }
}

bool Block::hasHiddenItems() const {
    return hiddenItemPrototype && hiddenItemCount > 0;
}

std::unique_ptr<Item> Block::releaseHiddenItem(Hero* hero) {
    if (!hero || !hasHiddenItems()) return nullptr;

    std::unique_ptr<Item> spawnedItem = hiddenItemPrototype->clone(hero);
    if (!spawnedItem) {
        hiddenItemCount = 0;
        hiddenItemPrototype.reset();
        return nullptr;
    }

    spawnedItem->spawn();
    --hiddenItemCount;
    if (hiddenItemCount == 0) {
        hiddenItemPrototype.reset();
    }
    return spawnedItem;
}

sf::FloatRect Block::getBounds() const {
    return hitbox.getGlobalBounds();
}

sf::RectangleShape& Block::getHitbox(){
    return hitbox;
}
