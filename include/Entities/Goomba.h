#ifndef GOOMBA_H
#define GOOMBA_H

#include "Enemy.h"

class Goomba : public Enemy {
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
