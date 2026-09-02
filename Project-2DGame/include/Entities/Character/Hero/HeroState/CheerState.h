#pragma once

#include "HeroState.h"

class CheerState : public HeroState {
public:
    CheerState() = default;
    ~CheerState() override = default;

    void enter(Hero* hero) override;
    void update(Hero* hero, float deltaTime) override;
    std::string getState() override;
};
