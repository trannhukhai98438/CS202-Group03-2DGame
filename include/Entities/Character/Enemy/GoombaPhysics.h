#ifndef GOOMBA_PHYSICS_H
#define GOOMBA_PHYSICS_H

class Goomba;
class Character;

class GoombaPhysics {
public:
    void checkObstacles(Goomba& goomba);
    void move(Goomba& goomba, float deltaTime);
    void takeDamage(Goomba& goomba, int damage);
    void onStomped(Goomba& goomba, Character* attacker);
};

#endif
