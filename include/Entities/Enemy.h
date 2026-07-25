#ifndef ENEMY_H
#define ENEMY_H

#include "Character.h"
#include "EnemyState.h"
#include <memory>

enum class MoveDirection {
    Left = -1,
    Right = 1
};

class Enemy : public Character {
protected:
    float speed;
    int health;
    MoveDirection currentDir;
    float startX;
    float patrolLeftBound;
    float patrolRightBound;
    std::unique_ptr<EnemyState> currentState;

public:
    Enemy(float startX, float startY, float moveSpeed = 50.0f, float patrolRange = 100.0f);
    virtual ~Enemy() = default;

    void changeState(std::unique_ptr<EnemyState> newState);
    EnemyState* getCurrentState() const { return currentState.get(); }
    std::string getStateName() const { return currentState ? currentState->getName() : "None"; }

    void update(float deltaTime) final override {
        if (!isAlive) return;

        if (currentState) {
            currentState->update(*this, deltaTime);
        } else {
            checkObstacles();
            move(deltaTime);
            applyAnimation();
        }
    }

    void render(sf::RenderWindow& window) override;

    float getSpeed() const override { return speed; }
    int getHealth() const override { return health; }
    int getDamageOnTouch() const override { return 1; }
    int getScoreValue() const override { return 100; }
    std::string getCharacterType() const override { return "Enemy"; }

    MoveDirection getDirection() const { return currentDir; }
    void setDirection(MoveDirection dir) { currentDir = dir; facingRight = (dir == MoveDirection::Right); }
    void flipDirection() { setDirection(currentDir == MoveDirection::Left ? MoveDirection::Right : MoveDirection::Left); }
    void setPatrolBounds(float minX, float maxX);
    float getPatrolLeftBound() const { return patrolLeftBound; }
    float getPatrolRightBound() const { return patrolRightBound; }

    void interactWith(Character* other) override;
    void onStomped(Character* attacker) override;
    void onSideCollision(Character* attacker) override;

    virtual void checkObstacles();
    virtual void move(float deltaTime);
    virtual void applyAnimation();
};

#endif