#pragma once
#include "HeroState.h"

class SlideState: public HeroState{
public:
    SlideState()=default;
    ~SlideState()=default;
    void enter(Hero* hero) override;
    void update(Hero* hero, float deltatime) override;
    std::string getState() override;
};