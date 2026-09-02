#ifndef WITCH_PHYSICS_H
#define WITCH_PHYSICS_H

class Witch;
class Character;

class WitchPhysics {
public:
    void checkObstacles(Witch& witch);
    void move(Witch& witch, float deltaTime);
    void takeDamage(Witch& witch, int damage);
    void onStomped(Witch& witch, Character* attacker);
    void updateCooldown(Witch& witch, float deltaTime);
};

#endif
