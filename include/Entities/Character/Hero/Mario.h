#pragma once
#include "Hero.h"

class Mario:public Hero{
public:
    Mario(float x, float y);
    ~Mario()=default;
    void specialAbility();
};