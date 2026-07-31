#pragma once
#include "HeroForm.h"

class FireForm: public HeroForm{
public:
    FireForm()=default;
    ~FireForm()=default;
    void enter(Hero* hero) override;
    void update(Hero* hero, float deltatime) override;
    std::string getForm() override;
    void specialAbility(Hero* hero) override;
};