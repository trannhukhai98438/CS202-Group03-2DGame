#ifndef KOOPA_H
#define KOOPA_H

#include "Enemy.h"

class Koopa : public Enemy {
private:
    bool isShell;

public:
    Koopa(float startX, float startY);

    void draw(sf::RenderWindow& window) override;

protected:
    void checkObstacles() override;
    void applyAnimation() override;
};

#endif