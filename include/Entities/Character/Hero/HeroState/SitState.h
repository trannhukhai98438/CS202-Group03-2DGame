#pragma once
#include "HeroState.h"

class SitState: public HeroState{
public:
    SitState()=default;
    ~SitState()=default;
    void enter(Hero* hero) override;
    void exit(Hero* hero) override;
    void update(Hero* hero, float deltatime) override;
    std::string getState() override;
};
