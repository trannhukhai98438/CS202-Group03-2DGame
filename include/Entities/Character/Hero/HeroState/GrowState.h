#pragma once
#include "HeroState.h"

class GrowState: public HeroState{
private:
    float timer;
    float maxTime;
public:
    GrowState();
    ~GrowState()=default;
    void enter(Hero* hero) override;
    void update(Hero* hero, float deltatime) override;
    std::string getState() override;
};
