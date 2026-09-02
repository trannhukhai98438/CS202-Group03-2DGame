#pragma once
#include "HeroState.h"

class ShrinkState: public HeroState{
private:
    float timer;
    float maxTime;
public:
    ShrinkState();
    ~ShrinkState()=default;
    void enter(Hero* hero) override;
    void update(Hero* hero, float deltatime) override;
    std::string getState() override;
};
