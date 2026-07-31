#pragma once
#include "HeroState.h"

class FlyState: public HeroState{
public:
    FlyState()=default;
    ~FlyState()=default;
    void enter(Hero* hero) override;
    void update(Hero* hero, float deltatime) override;
    std::string getState() override;
};