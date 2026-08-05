#ifndef GOOMBA_H
#define GOOMBA_H

#include "Enemy.h"
#include "GoombaPhysics.h"
#include "GoombaAnimator.h"

class Goomba : public Enemy {
private:
    GoombaPhysics physics;
    GoombaAnimator animatorComp;

public:
    Goomba(float startX, float startY, float patrolRange = 100.0f);

    int getDamageOnTouch() const override;
    int getScoreValue() const override { return 100; }
    std::string getCharacterType() const override { return "Goomba"; }

    void render(sf::RenderWindow& window) override;
    void takeDamage(int damage) override;
    void onStomped(BaseEntity* attacker) override;

    bool getIsSquished() const;

protected:
    void checkObstacles() override;
    void move(float deltaTime) override;
    void applyAnimation() override;
};

#endif
