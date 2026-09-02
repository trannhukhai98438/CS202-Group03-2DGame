#pragma once

#include "Entities/Character/Hero/HeroFactory.h"
#include "Gameplay/GameWorld.h"
#include "Gameplay/InteractionSystem.h"
#include "Gameplay/WorldPhysicsSystem.h"
#include "Managers/MapData.hpp"
#include "Managers/SoundManager.hpp"

#include <SFML/Graphics.hpp>
#include <cstddef>
#include <string>
#include <vector>

class Hero;

enum class PipeDirection {
    None,
    Down,
    Up,
    Left,
    Right
};

struct LevelUpdateResult {
    int scoreDelta{0};
    bool travelledThroughPipe{false};
};

// Facade for one running level. PlayingState owns presentation and state
// transitions; this class owns and advances the gameplay world.
class LevelRuntime {
public:
    LevelRuntime(const std::string& mapPath,
                 const std::string& tilesetPath,
                 HeroType heroType);
    
    void reload(const std::string& mapPath,
                const std::string& tilesetPath,
                HeroType heroType);
    
    void setSoundManager(SoundManager* sm) {
        m_soundManager = sm;
        m_world.setSoundManager(sm);
    }
    SoundManager* getSoundManager() const { return m_soundManager; }

    LevelUpdateResult update(float deltaTime,
                             PipeDirection pipeDirection = PipeDirection::None);
    void renderWorld(sf::RenderWindow& window);

    GameWorld& getWorld() { return m_world; }
    const GameWorld& getWorld() const { return m_world; }

    Hero* getHero();
    const Hero* getHero() const;
    float getWorldWidth() const;
    float getActiveRegionBottom() const;
    bool syncActiveRegionToHero();
    bool hasActivatedGoal() const;
    std::string getActivatedGoalBgm() const;
    bool isReady() const;

    const std::string& getMapPath() const { return m_mapPath; }
    const std::string& getTilesetPath() const { return m_tilesetPath; }

private:
    static constexpr std::size_t INVALID_REGION_INDEX =
        static_cast<std::size_t>(-1);

    struct PipeRoute {
        sf::FloatRect entrance;
        sf::Vector2f destination;
        PipeDirection direction{PipeDirection::None};
        std::size_t destinationRegionIndex{INVALID_REGION_INDEX};
    };

    void cachePlayableRegions();
    void cachePipeRoutes();
    bool tryTravelThroughPipe(PipeDirection direction);
    bool isAlignedWithPipe(const sf::FloatRect& heroBounds,
                           const PipeRoute& route) const;
    std::size_t findPlayableRegionAt(float worldX, float worldY) const;
    MapTheme findFallbackThemeAt(float worldX, float worldY) const;
    void setActiveRegion(std::size_t regionIndex);
    bool syncActiveRegionFromHeroMovement();
    bool isInsideActiveRegion(const sf::FloatRect& bounds) const;
    float getRegionBottom(std::size_t regionIndex) const;

    std::string m_mapPath;
    std::string m_tilesetPath;

    SoundManager* m_soundManager = nullptr;

    GameWorld m_world;
    WorldPhysicsSystem m_worldPhysics;
    InteractionSystem m_interactions;
    std::vector<MapObject> m_playableRegions;
    std::vector<PipeRoute> m_pipeRoutes;
    bool m_ready{false};
    std::size_t m_activeRegionIndex{INVALID_REGION_INDEX};
    float m_activeRegionBottom{0.0f};
    float m_pipeCooldownRemaining{0.0f};
};
