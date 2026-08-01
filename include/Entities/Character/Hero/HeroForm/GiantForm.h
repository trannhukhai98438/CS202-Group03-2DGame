#pragma once
#include "HeroForm.h"

class GiantForm: public HeroForm{
public:
    GiantForm()=default;
    ~GiantForm()=default;
    void enter(Hero* hero) override;
    void update(Hero* hero, float deltatime) override;
    std::string getForm() override;
    void takedamage(Hero* hero) override;
};