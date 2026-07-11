#pragma once

#include "Character.h"

enum class MarioState {
    Small,
    Super,
    Fire,
    Dead
};

class Mario:public Character{
    private:
        bool isInvulnerable;
        float invulnerableTimer;
        const float MAX_INVULNERABLE_TIME = 2.0f;
        MarioState currentState;
        float moveSpeed;
        float jumpVelocity;
    protected:
        // void specialAbility() override;
    public:
        Mario(float x, float y);

        void update(float deltatime) override;

        void render(sf::RenderWindow& window) override;

        void die() override;
};