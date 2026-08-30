#include "Gameplay/LevelRuntime.h"

#include "Entities/Block/Block.h"
#include "Entities/Character/Enemy/Enemy.h"
#include "Entities/Character/Enemy/Projectile.h"
#include "Entities/Character/Hero/Hero.h"
#include "Entities/Character/Hero/HeroState/IdleState.h"
#include "Entities/Character/Hero/HeroState/JumpState.h"
#include "Entities/Goal/LevelGoal.h"
#include "Entities/Item/Item.h"
#include "Gameplay/LevelBuilder.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <string>

namespace {
constexpr float PIPE_ALIGNMENT_EPSILON = 2.0f;
constexpr float PIPE_COOLDOWN_SECONDS = 0.3f;
constexpr float SURFACE_DEATH_PLANE = 720.0f;

PipeDirection parsePipeDirection(std::string direction) {
    std::transform(direction.begin(), direction.end(), direction.begin(),
                   [](unsigned char character) {
                       return static_cast<char>(std::tolower(character));
                   });

    if (direction == "down") return PipeDirection::Down;
    if (direction == "up") return PipeDirection::Up;
    if (direction == "left") return PipeDirection::Left;
    if (direction == "right") return PipeDirection::Right;
    return PipeDirection::None;
}
}

LevelRuntime::LevelRuntime(const std::string& mapPath,
                           const std::string& tilesetPath,
                           HeroType heroType) {
    LevelBuilder builder;
    m_ready = builder.build(m_world, mapPath, tilesetPath, heroType);
    if (m_ready) {
        cachePipeRoutes();
        m_activeRegionBottom = detectActiveRegionBottom();
    }
}

void LevelRuntime::reload(const std::string& mapPath,
                          const std::string& tilesetPath,
                          HeroType heroType) {
    m_pipeRoutes.clear();
    m_pipeCooldownRemaining = 0.0f;
    m_activeRegionBottom = 720.0f;
    m_ready = false;

    m_world.clear();

    LevelBuilder builder;
    m_ready = builder.build(m_world, mapPath, tilesetPath, heroType);

    if (m_ready) {
        cachePipeRoutes();
        m_activeRegionBottom = detectActiveRegionBottom();
    }
}

LevelUpdateResult LevelRuntime::update(float deltaTime,
                                       PipeDirection pipeDirection) {
    LevelUpdateResult result;
    if (!m_ready) return result;

    m_pipeCooldownRemaining = std::max(
        0.0f, m_pipeCooldownRemaining - deltaTime);

    if (Hero* hero = m_world.hero()) {
        // DeadState owns its death motion, so it must keep receiving updates.
        hero->update(deltaTime);
    }

    for (auto& block : m_world.blocks()) {
        block->update(deltaTime);
    }
    for (auto& item : m_world.items()) {
        item->update(deltaTime);
    }
    for (auto& enemy : m_world.enemies()) {
        enemy->update(deltaTime);
    }
    for (auto& projectile : m_world.projectiles()) {
        projectile->update(deltaTime);
    }

    m_worldPhysics.update(m_world, deltaTime);
    result.scoreDelta = m_interactions.resolve(m_world);

    // Pipe entrances are checked after collision resolution so the Hero has
    // already been snapped exactly onto the pipe mouth. A successful travel
    // changes the active room before its death plane is evaluated below.
    result.travelledThroughPipe = tryTravelThroughPipe(pipeDirection);

    Hero* hero = m_world.hero();
    if (hero && !hero->isDead()) {
        const sf::FloatRect heroBounds = hero->getBounds();
        const float heroFeet = heroBounds.top + heroBounds.height;
        const float deathPlane = std::max(
            SURFACE_DEATH_PLANE, m_activeRegionBottom);
        if (heroFeet > deathPlane) {
            // Preserve the original visible surface fall while extending the
            // boundary for rooms that reach below the surface viewport.
            hero->die();
        }
    }

    const float worldWidth = getWorldWidth();
    for (auto& projectile : m_world.projectiles()) {
        const sf::FloatRect bounds = projectile->getBounds();
        if (bounds.left + bounds.width < 0.0f
            || bounds.left > worldWidth
            || bounds.top > 900.0f) {
            projectile->die();
        }
    }

    m_world.removeInactiveEntities();
    return result;
}

void LevelRuntime::renderWorld(sf::RenderWindow& window) {
    if (!m_ready) return;

    window.draw(m_world.levelManager());

    for (auto& block : m_world.blocks()) {
        block->render(window);
    }
    for (auto& item : m_world.items()) {
        item->render(window);
    }
    if (Hero* hero = m_world.hero()) {
        hero->render(window);
    }
    for (auto& enemy : m_world.enemies()) {
        if (enemy) enemy->render(window);
    }
    for (auto& projectile : m_world.projectiles()) {
        if (projectile) projectile->render(window);
    }
}

Hero* LevelRuntime::getHero() {
    return m_world.hero();
}

const Hero* LevelRuntime::getHero() const {
    return m_world.hero();
}

float LevelRuntime::getWorldWidth() const {
    return static_cast<float>(m_world.levelManager().getMapWidthPixels());
}

float LevelRuntime::getActiveRegionBottom() const {
    return m_activeRegionBottom;
}

void LevelRuntime::cachePipeRoutes() {
    const LevelManager& level = m_world.levelManager();
    const auto entrances = level.getObjectsByClass("Trigger", "pipe_in");
    m_pipeRoutes.clear();

    for (const MapObject& entrance : entrances) {
        const MapObject* destination =
            level.getMapData().findPipeExit(entrance.name);
        const PipeDirection direction = parsePipeDirection(entrance.direction);
        if (!destination || direction == PipeDirection::None) continue;

        m_pipeRoutes.push_back({
            sf::FloatRect(entrance.x, entrance.y,
                          entrance.width, entrance.height),
            sf::Vector2f(destination->x, destination->y),
            direction
        });
    }
}

bool LevelRuntime::tryTravelThroughPipe(PipeDirection direction) {
    Hero* hero = m_world.hero();
    if (!hero || hero->isDead() || !hero->getGrounded()
        || direction == PipeDirection::None
        || m_pipeCooldownRemaining > 0.0f) {
        return false;
    }

    const std::string state = hero->getStateName();
    if (state != "Idle" && state != "Run" && state != "Sit") {
        return false;
    }

    for (const PipeRoute& route : m_pipeRoutes) {
        sf::FloatRect heroBounds = hero->getBounds();
        if (route.direction != direction
            || !isAlignedWithPipe(heroBounds, route)) {
            continue;
        }

        // Leaving Sit first restores the powered form's standing hitbox. The
        // pipe_out point is a centre-X/feet anchor, so every form exits with
        // the same feet position without overlapping the destination pipe.
        if (state == "Sit") {
            hero->setState(std::make_unique<IdleState>());
            heroBounds = hero->getBounds();
        }

        const float destinationLeft =
            route.destination.x - heroBounds.width * 0.5f;
        const float destinationTop =
            route.destination.y - heroBounds.height;
        hero->setPosition(destinationLeft, destinationTop);
        hero->setVelocity(0.0f, 0.0f);

        const bool destinationIsGrounded =
            m_world.levelManager().isSolidAtPixel(
                route.destination.x, route.destination.y + 0.5f);
        hero->setGrounded(destinationIsGrounded);
        if (destinationIsGrounded) {
            hero->setState(std::make_unique<IdleState>());
        } else {
            hero->setState(std::make_unique<JumpState>(AirEntry::Fell));
        }

        m_activeRegionBottom = detectActiveRegionBottom();
        m_pipeCooldownRemaining = PIPE_COOLDOWN_SECONDS;
        return true;
    }

    return false;
}

bool LevelRuntime::isAlignedWithPipe(const sf::FloatRect& heroBounds,
                                     const PipeRoute& route) const {
    const sf::FloatRect& entrance = route.entrance;
    const float heroRight = heroBounds.left + heroBounds.width;
    const float heroBottom = heroBounds.top + heroBounds.height;
    const float entranceRight = entrance.left + entrance.width;
    const float entranceBottom = entrance.top + entrance.height;

    const bool horizontallyContained =
        heroBounds.left >= entrance.left - PIPE_ALIGNMENT_EPSILON
        && heroRight <= entranceRight + PIPE_ALIGNMENT_EPSILON;
    const bool verticallyContained =
        heroBounds.top >= entrance.top - PIPE_ALIGNMENT_EPSILON
        && heroBottom <= entranceBottom + PIPE_ALIGNMENT_EPSILON;

    switch (route.direction) {
    case PipeDirection::Down:
        return horizontallyContained
            && std::abs(heroBottom - entranceBottom)
                <= PIPE_ALIGNMENT_EPSILON;
    case PipeDirection::Up:
        return horizontallyContained
            && std::abs(heroBounds.top - entrance.top)
                <= PIPE_ALIGNMENT_EPSILON;
    case PipeDirection::Left:
        return verticallyContained
            && std::abs(heroBounds.left - entrance.left)
                <= PIPE_ALIGNMENT_EPSILON;
    case PipeDirection::Right:
        return verticallyContained
            && std::abs(heroRight - entranceRight)
                <= PIPE_ALIGNMENT_EPSILON
            && std::abs(heroBottom - entranceBottom)
                <= PIPE_ALIGNMENT_EPSILON;
    case PipeDirection::None:
        return false;
    }

    return false;
}

float LevelRuntime::detectActiveRegionBottom() const {
    const Hero* hero = m_world.hero();
    const LevelManager& level = m_world.levelManager();
    if (!hero || level.getTileWidth() <= 0 || level.getTileHeight() <= 0
        || level.getMapWidthTiles() <= 0 || level.getMapHeightTiles() <= 0) {
        return m_activeRegionBottom;
    }

    const sf::FloatRect heroBounds = hero->getBounds();
    const float heroCenterX = heroBounds.left + heroBounds.width * 0.5f;
    const float heroFeet = heroBounds.top + heroBounds.height;
    const int tileWidth = level.getTileWidth();
    const int tileHeight = level.getTileHeight();

    const int tileX = std::clamp(
        static_cast<int>(std::floor(heroCenterX / tileWidth)),
        0,
        level.getMapWidthTiles() - 1);
    int tileY = std::clamp(
        static_cast<int>(std::floor(heroFeet / tileHeight)),
        0,
        level.getMapHeightTiles() - 1);

    // Locate the floor directly below the spawn point, then include every
    // contiguous solid tile beneath it. A blank row separates this room from
    // any vertically stacked room in the same TMJ.
    while (tileY < level.getMapHeightTiles()
           && !level.isSolidAtTile(tileX, tileY)) {
        ++tileY;
    }
    if (tileY >= level.getMapHeightTiles()) {
        return m_activeRegionBottom;
    }
    while (tileY + 1 < level.getMapHeightTiles()
           && level.isSolidAtTile(tileX, tileY + 1)) {
        ++tileY;
    }

    return static_cast<float>((tileY + 1) * tileHeight);
}

bool LevelRuntime::hasActivatedGoal() const {
    for (const auto& goal : m_world.goals()) {
        if (goal && goal->isActivated()) return true;
    }
    return false;
}

bool LevelRuntime::isReady() const {
    return m_ready;
}
