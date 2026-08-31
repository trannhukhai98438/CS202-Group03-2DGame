#pragma once

#include "Core/State.h"
#include "Gameplay/LevelRuntime.h"
#include "Managers/HUDManager.hpp"
#include "Managers/SoundManager.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

class PlayingState : public State {
public:
    explicit PlayingState(std::shared_ptr<HUDManager> hudManager, const std::string& levelPath = "assets/maps/levels/1-1.tmj");
    ~PlayingState() override;

    void processEvents(sf::Event& event) override;
    void update(sf::Time dt) override;
    void render(sf::RenderWindow& window) override;

private:
    static constexpr float DEFEAT_DELAY_SECONDS = 2.0f;

    sf::View m_camera;
    std::shared_ptr<HUDManager> m_hudManager;
    std::string m_levelPath;
    LevelRuntime m_levelRuntime;
    SoundManager m_soundManager;
    int m_lastCoinCount{0};
    PipeDirection m_latchedPipeDirection{PipeDirection::None};

    bool m_victoryPending{false};
    float m_victoryDelayRemaining{0.0f};
    bool m_defeatPending{false};
    float m_defeatDelayRemaining{0.0f};

    // Environmental VFX for Boss Phase 3
    struct GroundCrater {
        float x{0.0f};
        float y{0.0f};
        float remainingTime{2.5f};
        float totalTime{2.5f};
        float animTimer{0.0f};
    };

    sf::Texture m_vfxTexture;
    float m_vfxAnimTimer{0.0f};
    float m_lightningStrikeTimer{2.0f};
    bool  m_lightningActive{false};
    bool  m_lightningDamagedHero{false};
    float m_lightningX{600.0f};
    int   m_lightningFrame{0};
    float m_lightningFrameTimer{0.0f};
    std::vector<GroundCrater> m_groundCraters;
};
