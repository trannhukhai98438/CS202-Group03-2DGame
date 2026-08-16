#pragma once
#include <memory>
#include "Entities/Block/Block.h"
#include "Entities/Item/ItemFactory.h"

enum class BlockType {
    Brick,
    Question,
    Invisible,
    None
};

class BlockFactory {
public:
    // hiddenItem defaults to None, useful for BrickBlocks or empty QuestionBlocks
    static std::unique_ptr<Block> createBlock(BlockType type, float x, float y, ItemType hiddenItem = ItemType::None);
};
