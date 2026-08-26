#pragma once
#include "HeroState.h"

enum class AirEntry {
    Jumped,
    Fell
};

class JumpState: public HeroState{
private:
    AirEntry entry;
public:
    explicit JumpState(AirEntry entry);
    ~JumpState()=default;
    void enter(Hero* hero) override;
    void update(Hero* hero, float deltatime) override;
    std::string getState() override;
};
