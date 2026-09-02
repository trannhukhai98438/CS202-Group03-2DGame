#pragma once
#include "HeroState.h"

class RunState: public HeroState{
public:
    RunState()=default;
    ~RunState()=default;
    void enter(Hero* hero) override;
    void update(Hero* hero, float deltatime) override;
    std::string getState() override;
};