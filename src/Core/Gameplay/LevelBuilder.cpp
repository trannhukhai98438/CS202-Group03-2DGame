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

std::string getContainedItem(const MapObject& object) {
    const std::string contain = object.getProperty("contain", object.contain);
    if (!contain.empty() && contain != "none") return contain;

    const std::string legacyItem = object.getProperty("item", "coin");
    return legacyItem == "none" ? "coin" : legacyItem;
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
    bool hasSpawnPoint = false;
    const auto starts =
        world.levelManager().getObjectsByClass("Trigger", "start");
    if (!starts.empty()) {
        spawnPoint = starts.front();
        hasSpawnPoint = true;
    } else {
        hasSpawnPoint = world.levelManager().getObjectByName(
            "Objects", "SpawnPoint", spawnPoint);
    }

    auto hero = HeroFactory::createHero(
        heroType,
        hasSpawnPoint ? spawnPoint.x : 100.f,
        hasSpawnPoint ? spawnPoint.y : 384.f,
        spawnCallback);
    if (hero && hasSpawnPoint) {
        // Tiled point objects mark the character's foot position.
        hero->setPosition(spawnPoint.x,
                          spawnPoint.y - hero->getBounds().height);
    }
    world.setHero(std::move(hero));

    const int tileWidth = world.levelManager().getTileWidth();
    const int tileHeight = world.levelManager().getTileHeight();
    for (int y = 0; y < world.levelManager().getMapHeightTiles(); ++y) {
        for (int x = 0; x < world.levelManager().getMapWidthTiles(); ++x) {
            if (!world.levelManager().isSolidAtTile(x, y)) continue;

            sf::RectangleShape collider;
            collider.setSize({static_cast<float>(tileWidth),
                              static_cast<float>(tileHeight)});
            collider.setPosition(static_cast<float>(x * tileWidth),
                                 static_cast<float>(y * tileHeight));
            world.mapColliders().push_back(collider);
        }
    }

    bool hasGoalTrigger = false;
    const auto endTriggers =
        world.levelManager().getObjectsByClass("Trigger", "end");
    for (const auto& trigger : endTriggers) {
        if (trigger.width <= 0.f || trigger.height <= 0.f) continue;
        world.addGoal(std::make_unique<Flag>(sf::FloatRect(
            trigger.x, trigger.y, trigger.width, trigger.height)));
        hasGoalTrigger = true;
    }

    // Spawn Interactive blocks, ground coins and goals from the map object
    // layer. Tile objects use their bottom edge as Tiled's Y coordinate.
    BlockFactory blockFactory;
    const auto objects =
        world.levelManager().getObjectsFromLayer("Interactive");
    for (const auto& object : objects) {
        const float worldX = object.x;
        const float worldY = object.gid > 0
            ? object.y - object.height
            : object.y;

        const std::string& type = object.className;
        if (type == "brick") {
            world.addBlock(blockFactory.createBlock(
                BlockType::Brick, worldX, worldY));
        } else if (type == "question") {
            const std::string item = getContainedItem(object);
            world.addBlock(blockFactory.createBlock(
                BlockType::Question,
                worldX,
                worldY,
                getBlockItemType(item)));
        } else if (type == "invisible") {
            const std::string item = getContainedItem(object);
            world.addBlock(blockFactory.createBlock(
                BlockType::Invisible,
                worldX,
                worldY,
                getBlockItemType(item)));
        } else if (type == "coin") {
            auto coin = std::make_unique<Coin>(worldX, worldY);
            coin->spawnAsGroundCoin();
            world.addItem(std::move(coin));
        } else if (type == "flag" && !hasGoalTrigger) {
            const sf::FloatRect triggerBounds(
                worldX,
                worldY,
                object.width,
                object.height);
            world.addGoal(std::make_unique<Flag>(triggerBounds));
        }
    }

    const auto enemySpawners =
        world.levelManager().getObjectsFromLayer("Spawner");
    bool spawnedThorKing = false;
    for (const auto& spawner : enemySpawners) {
        std::unique_ptr<Enemy> enemy;
        if (spawner.className == "goomba") {
            enemy = EnemyFactory::createEnemy(
                EnemyType::Goomba, spawner.x, spawner.y, 150.f,
                spawnCallback);
        } else if (spawner.className == "koopa") {
            enemy = EnemyFactory::createEnemy(
                EnemyType::Koopa, spawner.x, spawner.y, 200.f,
                spawnCallback);
        } else if (spawner.className == "witch") {
            enemy = EnemyFactory::createEnemy(
                EnemyType::Witch, spawner.x, spawner.y, 150.f,
                spawnCallback);
        } else if (spawner.className == "thorking" || spawner.className == "ThorKing") {
            enemy = EnemyFactory::createEnemy(
                EnemyType::ThorKing, spawner.x, spawner.y, 600.f,
                spawnCallback);
            spawnedThorKing = true;
        }
        if (!enemy) continue;

        // Spawner point Y is a feet/baseline coordinate in Tiled.
        enemy->setPosition({
            spawner.x,
            spawner.y - enemy->getBounds().height
        });
        world.addEnemy(std::move(enemy));
    }

    // Programmatic fallback to ensure ThorKing is spawned at X=900 for testing
    if (!spawnedThorKing) {
        auto boss = EnemyFactory::createEnemy(
            EnemyType::ThorKing, 900.f, 200.f, 600.f,
            spawnCallback);
        if (boss) {
            world.addEnemy(std::move(boss));
        }
    }

    return world.hero() != nullptr;
}
