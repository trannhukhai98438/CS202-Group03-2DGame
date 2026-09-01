#pragma once

#include "Core/State.h"
#include "Gameplay/LevelRuntime.h"
#include "Managers/HUDManager.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

class PlayingState : public State {
public:
    explicit PlayingState(std::shared_ptr<HUDManager> hudManager);
    ~PlayingState() override;

    void processEvents(sf::Event& event) override;
    void update(sf::Time dt) override;
    void render(sf::RenderWindow& window) override;

private:
    void quickSave();
    void quickLoad();

    static constexpr float DEFEAT_DELAY_SECONDS = 2.0f;
    bool updateTimer(float deltaTime);

    sf::View m_camera;
    std::shared_ptr<HUDManager> m_hudManager;
    LevelRuntime m_levelRuntime;
    int m_lastCoinCount{0};
    int m_attemptStartScore{0};
    int m_attemptStartCoins{0};
    int m_attemptStartLives{0};
    PipeDirection m_latchedPipeDirection{PipeDirection::None};
    bool m_invincibilityBgmActive{false};

    bool m_victoryPending{false};
    float m_victoryDelayRemaining{0.0f};
    bool m_defeatPending{false};
    float m_defeatDelayRemaining{0.0f};
};
