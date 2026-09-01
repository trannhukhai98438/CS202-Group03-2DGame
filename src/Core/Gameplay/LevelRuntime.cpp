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
#include <iostream>
#include <string>
#include <utility>

namespace {
constexpr float PIPE_ALIGNMENT_EPSILON = 2.0f;
constexpr float PIPE_COOLDOWN_SECONDS = 0.3f;
constexpr float REGION_EDGE_EPSILON = 0.001f;

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
                           HeroType heroType)
    : m_mapPath(mapPath)
    , m_tilesetPath(tilesetPath)
{
    LevelBuilder builder;
    m_ready = builder.build(m_world, mapPath, tilesetPath, heroType);
    if (m_ready) {
        cachePlayableRegions();
        cachePipeRoutes();
        syncActiveRegionToHero();
    }
}

void LevelRuntime::reload(const std::string& mapPath,
                          const std::string& tilesetPath,
                          HeroType heroType) {
    m_mapPath = mapPath;
    m_tilesetPath = tilesetPath;

    m_playableRegions.clear();
    m_pipeRoutes.clear();
    m_pipeCooldownRemaining = 0.0f;
    m_activeRegionIndex = INVALID_REGION_INDEX;
    m_activeRegionBottom = 0.0f;
    m_ready = false;

    m_world.clear();

    LevelBuilder builder;
    m_ready = builder.build(m_world, mapPath, tilesetPath, heroType);

    if (m_ready) {
        cachePlayableRegions();
        cachePipeRoutes();
        syncActiveRegionToHero();
    }
}

LevelUpdateResult LevelRuntime::update(float deltaTime,
                                       PipeDirection pipeDirection) {
    LevelUpdateResult result;
    if (!m_ready) return result;

    m_pipeCooldownRemaining = std::max(
        0.0f, m_pipeCooldownRemaining - deltaTime);

    // Snapshot each enemy's room before movement. Looking the room up after
    // it crosses a shared edge could incorrectly attach a falling surface
    // enemy to an underground room below it.
    std::vector<std::pair<Enemy*, float>> enemyFallBoundaries;
    enemyFallBoundaries.reserve(m_world.enemies().size());
    for (const auto& enemy : m_world.enemies()) {
        if (!enemy || !enemy->getIsAlive()) continue;

        const std::string state = enemy->getStateName();
        if (state == "FlippingDeath" || state == "Squished") continue;

        const sf::FloatRect bounds = enemy->getBounds();
        const float centerX = bounds.left + bounds.width * 0.5f;
        const float feet = bounds.top + bounds.height;
        const std::size_t regionIndex = findPlayableRegionAt(
            centerX, feet - REGION_EDGE_EPSILON);
        enemyFallBoundaries.emplace_back(
            enemy.get(), getRegionBottom(regionIndex));
    }

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
    if (!result.travelledThroughPipe) {
        syncActiveRegionFromHeroMovement();
    }

    Hero* hero = m_world.hero();
    if (hero && !hero->isDead()) {
        const sf::FloatRect heroBounds = hero->getBounds();
        const float heroFeet = heroBounds.top + heroBounds.height;
        if (heroFeet > m_activeRegionBottom) {
            hero->die();
        }
    }

    for (const auto& [enemy, regionBottom] : enemyFallBoundaries) {
        if (!enemy || !enemy->getIsAlive()) continue;

        const std::string state = enemy->getStateName();
        if (state == "FlippingDeath" || state == "Squished") continue;

        const sf::FloatRect enemyBounds = enemy->getBounds();
        const float enemyFeet = enemyBounds.top + enemyBounds.height;
        if (enemyFeet > regionBottom) {
            enemy->changeState(std::make_unique<FlippingDeathState>(
                std::max(0.0f, enemy->getVelocity().y)));
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
        if (block && isInsideActiveRegion(block->getBounds())) {
            block->render(window);
        }
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

bool LevelRuntime::syncActiveRegionToHero() {
    const Hero* hero = m_world.hero();
    if (!hero) {
        setActiveRegion(INVALID_REGION_INDEX);
        return false;
    }

    const sf::FloatRect bounds = hero->getBounds();
    const float centerX = bounds.left + bounds.width * 0.5f;
    const float feet = bounds.top + bounds.height;
    const std::size_t regionIndex = findPlayableRegionAt(
        centerX, feet - REGION_EDGE_EPSILON);

    if (regionIndex == INVALID_REGION_INDEX) {
        setActiveRegion(INVALID_REGION_INDEX);
        std::cerr << "[LevelRuntime] WARNING: Hero is not inside a unique "
                  << "Trigger/playable_region; using map bounds.\n";
        return false;
    }

    setActiveRegion(regionIndex);
    return true;
}

void LevelRuntime::cachePlayableRegions() {
    m_playableRegions.clear();
    const auto regions = m_world.levelManager().getObjectsByClass(
        "Trigger", "playable_region");

    for (const MapObject& region : regions) {
        if (region.width <= 0.0f || region.height <= 0.0f) {
            std::cerr << "[LevelRuntime] WARNING: Ignoring playable_region "
                      << region.id << " with invalid dimensions.\n";
            continue;
        }
        if (region.theme == MapTheme::Unspecified) {
            std::cerr << "[LevelRuntime] WARNING: playable_region "
                      << region.id
                      << " has no valid theme; Overworld will be used.\n";
        }
        m_playableRegions.push_back(region);
    }

    for (std::size_t first = 0; first < m_playableRegions.size(); ++first) {
        const MapObject& firstRegion = m_playableRegions[first];
        const sf::FloatRect firstBounds(
            firstRegion.x, firstRegion.y,
            firstRegion.width, firstRegion.height);

        for (std::size_t second = first + 1;
             second < m_playableRegions.size(); ++second) {
            const MapObject& secondRegion = m_playableRegions[second];
            const sf::FloatRect secondBounds(
                secondRegion.x, secondRegion.y,
                secondRegion.width, secondRegion.height);
            if (firstBounds.intersects(secondBounds)) {
                std::cerr << "[LevelRuntime] WARNING: playable_region "
                          << firstRegion.id << " overlaps region "
                          << secondRegion.id
                          << "; points in the overlap are ambiguous.\n";
            }
        }
    }

    if (m_playableRegions.empty()) {
        std::cerr << "[LevelRuntime] WARNING: No valid "
                  << "Trigger/playable_region found; using map bounds.\n";
    }

    setActiveRegion(INVALID_REGION_INDEX);
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

        const std::size_t destinationRegionIndex = findPlayableRegionAt(
            destination->x, destination->y);
        if (destinationRegionIndex == INVALID_REGION_INDEX) {
            std::cerr << "[LevelRuntime] WARNING: pipe_out '"
                      << destination->name
                      << "' is not inside a unique playable_region.\n";
        }

        m_pipeRoutes.push_back({
            sf::FloatRect(entrance.x, entrance.y,
                          entrance.width, entrance.height),
            sf::Vector2f(destination->x, destination->y),
            direction,
            destinationRegionIndex
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

        if (route.destinationRegionIndex != INVALID_REGION_INDEX) {
            setActiveRegion(route.destinationRegionIndex);
        } else {
            syncActiveRegionToHero();
        }
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

std::size_t LevelRuntime::findPlayableRegionAt(float worldX,
                                                float worldY) const {
    std::size_t match = INVALID_REGION_INDEX;

    for (std::size_t index = 0; index < m_playableRegions.size(); ++index) {
        const MapObject& region = m_playableRegions[index];
        const bool containsPoint =
            worldX >= region.x
            && worldX < region.x + region.width
            && worldY >= region.y
            && worldY < region.y + region.height;
        if (!containsPoint) continue;

        if (match != INVALID_REGION_INDEX) {
            return INVALID_REGION_INDEX;
        }
        match = index;
    }

    return match;
}

void LevelRuntime::setActiveRegion(std::size_t regionIndex) {
    m_activeRegionIndex = regionIndex;
    m_activeRegionBottom = getRegionBottom(regionIndex);

    MapTheme theme = MapTheme::Overworld;
    if (regionIndex < m_playableRegions.size()
        && m_playableRegions[regionIndex].theme != MapTheme::Unspecified) {
        theme = m_playableRegions[regionIndex].theme;
    }
    m_world.blockThemePalette().setActiveTheme(theme);
}

bool LevelRuntime::syncActiveRegionFromHeroMovement() {
    const Hero* hero = m_world.hero();
    if (!hero || hero->isDead()
        || m_activeRegionIndex >= m_playableRegions.size()) {
        return false;
    }

    const sf::FloatRect heroBounds = hero->getBounds();
    const float centerX = heroBounds.left + heroBounds.width * 0.5f;
    const float centerY = heroBounds.top + heroBounds.height * 0.5f;
    const std::size_t nextRegionIndex = findPlayableRegionAt(
        centerX, centerY);
    if (nextRegionIndex == INVALID_REGION_INDEX
        || nextRegionIndex == m_activeRegionIndex) {
        return false;
    }

    // Direct movement may cross between side-by-side regions. Vertically
    // stacked rooms only touch at an edge, so rejecting them here prevents a
    // pit fall from being mistaken for entering the room below. Pipe travel
    // switches stacked rooms explicitly via its destination region.
    const MapObject& currentRegion =
        m_playableRegions[m_activeRegionIndex];
    const MapObject& nextRegion = m_playableRegions[nextRegionIndex];
    const float verticalOverlap = std::min(
        currentRegion.y + currentRegion.height,
        nextRegion.y + nextRegion.height)
        - std::max(currentRegion.y, nextRegion.y);
    if (verticalOverlap <= 0.0f) return false;

    setActiveRegion(nextRegionIndex);
    return true;
}

bool LevelRuntime::isInsideActiveRegion(const sf::FloatRect& bounds) const {
    if (m_activeRegionIndex >= m_playableRegions.size()) return true;

    const MapObject& region = m_playableRegions[m_activeRegionIndex];
    const float centerX = bounds.left + bounds.width * 0.5f;
    const float centerY = bounds.top + bounds.height * 0.5f;
    return centerX >= region.x
        && centerX < region.x + region.width
        && centerY >= region.y
        && centerY < region.y + region.height;
}

float LevelRuntime::getRegionBottom(std::size_t regionIndex) const {
    if (regionIndex < m_playableRegions.size()) {
        const MapObject& region = m_playableRegions[regionIndex];
        return region.y + region.height;
    }

    return static_cast<float>(
        m_world.levelManager().getMapHeightPixels());
}

bool LevelRuntime::hasActivatedGoal() const {
    for (const auto& goal : m_world.goals()) {
        if (goal && goal->isActivated()) return true;
    }
    return false;
}

std::string LevelRuntime::getActivatedGoalBgm() const {
    for (const auto& goal : m_world.goals()) {
        if (goal && goal->isActivated()) {
            return goal->getCompletionBgm();
        }
    }
    return {};
}

bool LevelRuntime::isReady() const {
    return m_ready;
}
