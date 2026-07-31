#pragma once
#include "HeroState.h"

class JumpState: public HeroState{
public:
    JumpState()=default;
    ~JumpState()=default;
    void enter(Hero* hero) override;
    void update(Hero* hero, float deltatime) override;
    std::string getState() override;
};