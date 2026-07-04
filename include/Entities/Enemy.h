#ifndef ENEMY_H
#define ENEMY_H

#include "Character.h"

enum class MoveDirection {
    Left = -1,
    Right = 1
};

class Enemy : public Character {
protected:
    float speed;
    int blood;
    MoveDirection currentDir;

public:
    Enemy(float startX, float startY, float moveSpeed);

    virtual ~Enemy() = default;

    void update(float deltaTime) final override {
        if (!isAlive) return;

        checkObstacles();
        move(deltaTime);
        applyAnimation();
    }

protected:
    virtual void checkObstacles() = 0;
    virtual void applyAnimation() = 0;

    virtual void move(float deltaTime) {
        position.x += speed * direction.Dir_X * deltaTime;
        position.y += speed * direction.Dir_Y * deltaTime;
    }
};

#endif