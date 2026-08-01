#pragma once
#include "HeroForm.h"

class FireForm: public HeroForm{
public:
    FireForm();
    ~FireForm()=default;
    void enter(Hero* hero) override;
    void update(Hero* hero, float deltatime) override;
    std::string getForm() override;
    void takedamage(Hero* hero) override;
    void specialAbility(Hero* hero) override;
private:
    const float cooldown;
    float counttime;
};