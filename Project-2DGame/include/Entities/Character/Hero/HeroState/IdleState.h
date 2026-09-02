#pragma once
#include "HeroState.h"

class IdleState: public HeroState{
public:
    IdleState()=default;
    ~IdleState()=default;
    void enter(Hero* hero) override;
    void update(Hero* hero, float deltatime) override;
    std::string getState() override;
};