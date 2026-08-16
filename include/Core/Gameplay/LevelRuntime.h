#pragma once

#include "Entities/Character/Hero/HeroFactory.h"
#include "Gameplay/GameWorld.h"
#include "Gameplay/InteractionSystem.h"
#include "Gameplay/WorldPhysicsSystem.h"

#include <SFML/Graphics.hpp>
#include <string>

class Hero;

// Facade for one running level. PlayingState owns presentation and state
// transitions; this class owns and advances the gameplay world.
class LevelRuntime {
public:
    LevelRuntime(const std::string& mapPath,
                 const std::string& tilesetPath,
                 HeroType heroType);

    int update(float deltaTime);
    void renderWorld(sf::RenderWindow& window);

    Hero* getHero();
    const Hero* getHero() const;
    float getWorldWidth() const;
    bool hasActivatedGoal() const;
    bool isReady() const;

private:
    GameWorld m_world;
    WorldPhysicsSystem m_worldPhysics;
    InteractionSystem m_interactions;
    bool m_ready{false};
};
