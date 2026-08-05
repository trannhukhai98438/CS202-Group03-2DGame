#include "Entities/Block/Block.h"

Block::Block(float x, float y) : position(x, y), isHit(false), isActive(true), animator(sprite) {
    hitbox.setSize({16.f, 16.f});
    sprite.setOrigin(8.f, 16.f); // Bottom-center origin
    hitbox.setPosition(position);
    sprite.setPosition(position.x + 8.f, position.y + 16.f);
}

void Block::setItemPrototype(std::unique_ptr<Item> itemProto) {
    hiddenItemPrototype = std::move(itemProto);
}

sf::FloatRect Block::getBounds() const {
    return hitbox.getGlobalBounds();
}

sf::RectangleShape& Block::getHitbox(){
    return hitbox;
}