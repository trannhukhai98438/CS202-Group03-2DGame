#pragma once

#include "Entities/Character/Hero/HeroFactory.h"
#include "Gameplay/GameWorld.h"
#include "Gameplay/InteractionSystem.h"
#include "Gameplay/WorldPhysicsSystem.h"

#include <SFML/Graphics.hpp>
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

    LevelUpdateResult update(float deltaTime,
                             PipeDirection pipeDirection = PipeDirection::None);
    void renderWorld(sf::RenderWindow& window);

    Hero* getHero();
    const Hero* getHero() const;
    float getWorldWidth() const;
    float getActiveRegionBottom() const;
    bool hasActivatedGoal() const;
    bool isReady() const;

private:
    struct PipeRoute {
        sf::FloatRect entrance;
        sf::Vector2f destination;
        PipeDirection direction{PipeDirection::None};
    };

    void cachePipeRoutes();
    bool tryTravelThroughPipe(PipeDirection direction);
    bool isAlignedWithPipe(const sf::FloatRect& heroBounds,
                           const PipeRoute& route) const;
    float detectActiveRegionBottom() const;

    GameWorld m_world;
    WorldPhysicsSystem m_worldPhysics;
    InteractionSystem m_interactions;
    std::vector<PipeRoute> m_pipeRoutes;
    bool m_ready{false};
    float m_activeRegionBottom{720.0f};
    float m_pipeCooldownRemaining{0.0f};
};
