#pragma once
#include <string>

class Hero;


class HeroState{
public:
    HeroState()=default;
    virtual ~HeroState()=default;
    virtual void enter(Hero* hero)=0;
    virtual void exit(Hero* hero){}
    virtual void update(Hero* hero, float deltatime)=0;
    virtual std::string getState()=0;
};