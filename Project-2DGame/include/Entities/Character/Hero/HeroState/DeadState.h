#pragma once
#include "HeroState.h"

class DeadState: public HeroState{
public:
    DeadState()=default;
    ~DeadState()=default;
    void enter(Hero* hero) override;
    void update(Hero* hero, float deltatime) override;
    std::string getState() override;
};