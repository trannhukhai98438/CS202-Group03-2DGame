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

class BlockThemePalette;

class BlockFactory {
public:
    explicit BlockFactory(const BlockThemePalette& themePalette);

    // hiddenItem defaults to None, useful for BrickBlocks or empty QuestionBlocks
    std::unique_ptr<Block> createBlock(
        BlockType type,
        float x,
        float y,
        ItemType hiddenItem = ItemType::None,
        int itemCount = 1) const;

private:
    const BlockThemePalette& themePalette;
};
