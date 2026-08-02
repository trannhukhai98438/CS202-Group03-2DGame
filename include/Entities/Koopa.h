#ifndef KOOPA_H
#define KOOPA_H

#include "Enemy.h"

class Koopa : public Enemy {
private:
    float shellSpeed;

public:
    Koopa(float startX, float startY, float patrolRange = 120.0f);

    float getSpeed() const override;
    int getDamageOnTouch() const override;
    int getScoreValue() const override { return 200; }
    std::string getCharacterType() const override { return "Koopa"; }

    void render(sf::RenderWindow& window) override;
    void takeDamage(int damage) override;
    void becomeStaticShell();
    void onStomped(BaseEntity* attacker) override;
    void onSideCollision(BaseEntity* attacker) override;

    bool getIsShell() const;
    bool getIsShellSpinning() const;
    void kickShell(MoveDirection dir);

protected:
    void checkObstacles() override;
    void move(float deltaTime) override;
    void applyAnimation() override;
};

#endif
