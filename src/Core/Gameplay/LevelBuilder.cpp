#include "Gameplay/LevelBuilder.h"

#include "Entities/Block/BlockFactory.h"
#include "Entities/Character/Enemy/EnemyFactory.h"
#include "Entities/Goal/Flag.h"
#include "Entities/Item/Coin.h"
#include "Gameplay/GameWorld.h"

#include <iostream>
#include <memory>
#include <utility>

namespace {
ItemType getBlockItemType(const std::string& itemName) {
    if (itemName == "star") {
        return ItemType::Star;
    }
    if (itemName == "mushroom" || itemName == "flower") {
        return ItemType::PowerUpPrototype;
    }
    return ItemType::Coin;
}
}

bool LevelBuilder::build(GameWorld& world,
                         const std::string& mapPath,
                         const std::string& tilesetPath,
                         HeroType heroType) const {
    if (!world.levelManager().loadLevel(mapPath, tilesetPath)) {
        std::cerr << "[LevelBuilder] ERROR: Cannot load level!" << std::endl;
        return false;
    }

    auto spawnCallback = [&world](std::unique_ptr<Projectile> projectile) {
        world.addProjectile(std::move(projectile));
    };

    MapObject spawnPoint;
    if (world.levelManager().getObjectByName("Objects", "SpawnPoint",
                                              spawnPoint)) {
        world.setHero(HeroFactory::createHero(
            heroType,
            spawnPoint.x * 2.f,
            spawnPoint.y * 2.f + 272.f,
            spawnCallback));
    } else {
        world.setHero(HeroFactory::createHero(heroType, 100.f, 500.f,
                                              spawnCallback));
    }

    const int tileWidth = world.levelManager().getTileWidth();
    const int tileHeight = world.levelManager().getTileHeight();
    for (int y = 0; y < world.levelManager().getMapHeightTiles(); ++y) {
        for (int x = 0; x < world.levelManager().getMapWidthTiles(); ++x) {
            if (!world.levelManager().isSolidAtTile(x, y)) continue;

            sf::RectangleShape collider;
            collider.setSize({tileWidth * 2.f, tileHeight * 2.f});
            collider.setPosition(x * tileWidth * 2.f,
                                 y * tileHeight * 2.f + 272.f);
            world.mapColliders().push_back(collider);
        }
    }

    // Spawn Interactive blocks, ground coins and goals from the map object
    // layer. Tile objects use their bottom edge as Tiled's Y coordinate.
    BlockFactory blockFactory;
    const auto objects =
        world.levelManager().getObjectsFromLayer("Interactive");
    for (const auto& object : objects) {
        const float worldX = object.x * 2.f;
        const float worldY = object.gid > 0
            ? (object.y - object.height) * 2.f + 272.f
            : object.y * 2.f + 272.f;

        const std::string& type = object.className;
        if (type == "brick") {
            world.addBlock(blockFactory.createBlock(
                BlockType::Brick, worldX, worldY));
        } else if (type == "question") {
            const std::string item = object.getProperty("item", "coin");
            world.addBlock(blockFactory.createBlock(
                BlockType::Question,
                worldX,
                worldY,
                getBlockItemType(item)));
        } else if (type == "invisible") {
            const std::string item = object.getProperty("item", "coin");
            world.addBlock(blockFactory.createBlock(
                BlockType::Invisible,
                worldX,
                worldY,
                getBlockItemType(item)));
        } else if (type == "coin") {
            auto coin = std::make_unique<Coin>(worldX, worldY);
            coin->spawnAsGroundCoin();
            world.addItem(std::move(coin));
        } else if (type == "flag") {
            const sf::FloatRect triggerBounds(
                worldX,
                worldY,
                object.width * 2.f,
                object.height * 2.f);
            world.addGoal(std::make_unique<Flag>(triggerBounds));
        }
    }

    // TEMPORARY: Remove this block when the "Enemies"
    // object layer becomes available in the map.
    world.addEnemy(EnemyFactory::createEnemy(
        EnemyType::Goomba, 300.f, 624.f, 150.f, spawnCallback));
    world.addEnemy(EnemyFactory::createEnemy(
        EnemyType::Koopa, 600.f, 608.f, 200.f, spawnCallback));
    world.addEnemy(EnemyFactory::createEnemy(
        EnemyType::Witch, 900.f, 560.f, 150.f, spawnCallback));

    return world.hero() != nullptr;
}
