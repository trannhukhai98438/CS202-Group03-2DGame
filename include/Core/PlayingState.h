#pragma once

#include "Core/State.h"
#include "Gameplay/LevelRuntime.h"
#include "Managers/HUDManager.hpp"

#include <SFML/Graphics.hpp>

class PlayingState : public State {
public:
    PlayingState();
    ~PlayingState() override;

    void processEvents(sf::Event& event) override;
    void update(sf::Time dt) override;
    void render(sf::RenderWindow& window) override;

private:
    static constexpr float DEFEAT_DELAY_SECONDS = 2.0f;

    sf::View m_camera;
    LevelRuntime m_levelRuntime;
    HUDManager m_hudManager;
    int m_lastCoinCount{0};

    bool m_victoryPending{false};
    float m_victoryDelayRemaining{0.0f};
    bool m_defeatPending{false};
    float m_defeatDelayRemaining{0.0f};
};
