#pragma once
#include "HeroForm.h"

class SmallForm: public HeroForm{
public:
    SmallForm()=default;
    ~SmallForm()=default;
    void enter(Hero* hero) override;
    void update(Hero* hero, float deltatime) override;
    std::string getForm() override;
};