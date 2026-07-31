#pragma once
#include "Hero.h"

class Luigi:public Hero{
public:
    Luigi(float x, float y);
    ~Luigi()=default;
    virtual void specialAbility();
};