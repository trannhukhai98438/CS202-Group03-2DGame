#include "Gameplay/LevelBuilder.h"

#include "Entities/Block/BlockFactory.h"
#include "Entities/Block/Lifter.h"
#include "Entities/Character/Enemy/EnemyFactory.h"
#include "Entities/Character/Enemy/ThorKing.h"
#include "Entities/Goal/Flag.h"
#include "Entities/Goal/Princess.h"
#include "Entities/Item/Coin.h"
#include "Gameplay/GameWorld.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <iostream>
#include <memory>
#include <utility>

namespace {
ItemType getBlockItemType(const std::string& itemName, ItemType fallback) {
    std::string lower = itemName;
    std::transform(
        lower.begin(),
        lower.end(),
        lower.begin(),
        [](unsigned char character) {
            return static_cast<char>(std::tolower(character));
        });

    if (lower == "coin") {
        return ItemType::Coin;
    }
    if (lower == "star") {
        return ItemType::Star;
    }
    if (lower == "mushroom" || lower == "flower" || lower == "powerup" || lower == "powerupprototype") {
        return ItemType::PowerUpPrototype;
    }
    if (lower == "none" || lower.empty()) {
        return ItemType::None;
    }
    return fallback;
}

std::string getContainedItem(const MapObject& object,
                             const std::string& defaultItem) {
    const auto contain = object.properties.find("contain");
    if (contain != object.properties.end()) return contain->second;
    if (!object.contain.empty() && object.contain != "none") {
        return object.contain;
    }

    const auto legacyItem = object.properties.find("item");
    return legacyItem != object.properties.end()
        ? legacyItem->second
        : defaultItem;
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

        const sf::FloatRect triggerBounds(
            trigger.x, trigger.y, trigger.width, trigger.height);
        if (trigger.getProperty("goal") == "princess") {
            world.addGoal(std::make_unique<Princess>(triggerBounds));
        } else {
            world.addGoal(std::make_unique<Flag>(triggerBounds));
        }
        hasGoalTrigger = true;
    }

    // Spawn Interactive blocks, ground coins and goals from the map object
    // layer. Tile objects use their bottom edge as Tiled's Y coordinate.
    BlockFactory blockFactory(world.blockThemePalette());
    const auto objects =
        world.levelManager().getObjectsFromLayer("Interactive");
    for (const auto& object : objects) {
        const float worldX = object.x;
        const float worldY = object.gid > 0
            ? object.y - object.height
            : object.y;

        const std::string& type = object.className;
        if (type == "brick" || type == "Brick") {
            const std::string item = getContainedItem(object, "none");
            const ItemType defaultType = (item == "none" || item.empty()) ? ItemType::None : ItemType::Coin;
            world.addBlock(blockFactory.createBlock(
                BlockType::Brick,
                worldX,
                worldY,
                getBlockItemType(item, defaultType),
                std::max(0, object.count)));
        } else if (type == "question" || type == "Question") {
            const std::string item = getContainedItem(object, "coin");
            world.addBlock(blockFactory.createBlock(
                BlockType::Question,
                worldX,
                worldY,
                getBlockItemType(item, ItemType::Coin),
                std::max(0, object.count)));
        } else if (type == "invisible" || type == "Invisible") {
            const std::string item = getContainedItem(object, "coin");
            world.addBlock(blockFactory.createBlock(
                BlockType::Invisible,
                worldX,
                worldY,
                getBlockItemType(item, ItemType::Coin),
                std::max(0, object.count)));
        } else if (type == "coin" || type == "Coin") {
            auto coin = std::make_unique<Coin>(worldX, worldY);
            coin->spawnAsGroundCoin();
            world.addItem(std::move(coin));
        } else if ((type == "flag" || type == "Flag") && !hasGoalTrigger) {
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
    const auto lifterBoundaries =
        world.levelManager().getObjectsByClass(
            "Trigger", "platform_despawn");
    for (const auto& spawner : enemySpawners) {
        const bool isUpLifter = spawner.className == "platform_up";
        const bool isDownLifter = spawner.className == "platform_down";
        if (isUpLifter || isDownLifter) {
            if (spawner.width <= 0.0f || spawner.height <= 0.0f) {
                std::cerr << "[LevelBuilder] WARNING: Lifter "
                          << spawner.id << " has invalid dimensions."
                          << std::endl;
                continue;
            }

            const MapObject* matchedBoundary = nullptr;
            float closestDistance = -1.0f;
            bool hasAmbiguousBoundary = false;

            for (const auto& boundary : lifterBoundaries) {
                if (boundary.width <= 0.0f) continue;

                const float horizontalOverlap = std::min(
                    spawner.x + spawner.width,
                    boundary.x + boundary.width)
                    - std::max(spawner.x, boundary.x);
                if (horizontalOverlap <= 0.0f) continue;

                const float verticalOffset = boundary.y - spawner.y;
                if ((isUpLifter && verticalOffset >= 0.0f)
                    || (isDownLifter && verticalOffset <= 0.0f)) {
                    continue;
                }

                const float distance = std::abs(verticalOffset);
                if (closestDistance < 0.0f
                    || distance < closestDistance - 0.001f) {
                    matchedBoundary = &boundary;
                    closestDistance = distance;
                    hasAmbiguousBoundary = false;
                } else if (std::abs(distance - closestDistance) <= 0.001f) {
                    hasAmbiguousBoundary = true;
                }
            }

            if (!matchedBoundary || hasAmbiguousBoundary) {
                std::cerr << "[LevelBuilder] WARNING: Cannot resolve a unique "
                          << "boundary for Lifter " << spawner.id << '.'
                          << std::endl;
                continue;
            }

            const float topBoundary =
                std::min(spawner.y, matchedBoundary->y);
            const float bottomBoundary =
                std::max(spawner.y, matchedBoundary->y);
            world.addBlock(std::make_unique<Lifter>(
                spawner.x,
                spawner.y,
                spawner.width,
                spawner.height,
                topBoundary,
                bottomBoundary,
                isUpLifter,
                world.blockThemePalette()));
            continue;
        }

        std::unique_ptr<Enemy> enemy;
        if (spawner.className == "goomba" || spawner.className == "Goomba") {
            enemy = EnemyFactory::createEnemy(
                EnemyType::Goomba, spawner.x, spawner.y, 150.f,
                spawnCallback);
        } else if (spawner.className == "koopa" || spawner.className == "Koopa") {
            enemy = EnemyFactory::createEnemy(
                EnemyType::Koopa, spawner.x, spawner.y, 200.f,
                spawnCallback);
        } else if (spawner.className == "witch" || spawner.className == "Witch") {
            enemy = EnemyFactory::createEnemy(
                EnemyType::Witch, spawner.x, spawner.y, 150.f,
                spawnCallback);
        } else if (spawner.className == "thorking"
                   || spawner.className == "ThorKing"
                   || spawner.className == "thor_king"
                   || spawner.className == "boss"
                   || spawner.className == "Boss") {
            // Level 1-3 uses the generic boss marker at the end of its
            // dedicated boss_arena. ThorKing is the current level boss.
            enemy = EnemyFactory::createEnemy(
                EnemyType::ThorKing, spawner.x, spawner.y, 150.f,
                spawnCallback);
            if (auto boss = dynamic_cast<ThorKing*>(enemy.get())) {
                boss->setSoundCallback([&world](BossSoundEvent event) {
                    SoundManager* sm = world.getSoundManager();
                    if (!sm) {
                        return; // Hoặc log cảnh báo nếu vẫn null
                    }

                    if (event == BossSoundEvent::Attack) {
                        sm->playSFX("bowser_fire");
                    } else if (event == BossSoundEvent::Defeated) {
                        sm->playSFX("bowser_falls");
                    }
                });
            }
        }
        if (!enemy) continue;

        // Spawner point Y is a feet/baseline coordinate in Tiled.
        enemy->setPosition({
            spawner.x,
            spawner.y - enemy->getBounds().height
        });
        world.addEnemy(std::move(enemy));
    }

    return world.hero() != nullptr;
}
