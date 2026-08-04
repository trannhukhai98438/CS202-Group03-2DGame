#include "BlockFactory.h"
#include "Entities/Block/BrickBlock.h"
#include "Entities/Block/QuestionBlock.h"

std::unique_ptr<Block> BlockFactory::createBlock(BlockType type, float x, float y, ItemType hiddenItem) {
    switch (type) {
        case BlockType::Brick:
            // BrickBlock currently doesn't hold items, so we ignore hiddenItem
            return std::make_unique<BrickBlock>(x, y);
            
        case BlockType::Question: {
            auto qBlock = std::make_unique<QuestionBlock>(x, y);
            if (hiddenItem != ItemType::None) {
                qBlock->setItemPrototype(ItemFactory::createItem(hiddenItem, x, y));
            }
            return qBlock;
        }
            
        case BlockType::None:
        default:
            return nullptr;
    }
}
