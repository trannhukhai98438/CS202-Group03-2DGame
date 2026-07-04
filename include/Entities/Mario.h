#pragma once

#include "Character.h"

class Mario:public Character{
    private:
        bool isUntargetable;
    protected:
        // void specialAbility() override;
    public:
        Mario(float x, float y);

        void update(float deltatime) override;

        void render(sf::RenderWindow& window) override;

        void takedamage(int x) override;
};