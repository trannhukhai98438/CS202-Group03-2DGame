#include "Entities/Block/BlockFactory.h"
#include "Entities/Block/BrickBlock.h"
#include "Entities/Block/InvisibleBlock.h"
#include "Entities/Block/QuestionBlock.h"

std::unique_ptr<Block> BlockFactory::createBlock(
    BlockType type,
    float x,
    float y,
    ItemType hiddenItem,
    int itemCount) {
    std::unique_ptr<Block> block;
    switch (type) {
        case BlockType::Brick:
            block = std::make_unique<BrickBlock>(x, y);
            break;
            
        case BlockType::Question:
            block = std::make_unique<QuestionBlock>(x, y);
            break;

        case BlockType::Invisible:
            block = std::make_unique<InvisibleBlock>(x, y);
            break;
            
        case BlockType::None:
        default:
            return nullptr;
    }

    if (hiddenItem != ItemType::None && itemCount > 0) {
        block->setItemPrototype(
            ItemFactory::createItem(hiddenItem, x, y), itemCount);
    }
    return block;
}
