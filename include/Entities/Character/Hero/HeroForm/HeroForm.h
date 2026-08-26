#pragma once
#include <string>


class Hero;
class HeroForm{
public:
    HeroForm()=default;
    virtual ~HeroForm()=default;
    virtual void enter(Hero* hero)=0;
    virtual void update(Hero* hero ,float deltatime)=0;
    virtual std::string getForm()=0;
    virtual void takedamage(Hero* hero) = 0;
};
