#ifndef KOOPA_PHYSICS_H
#define KOOPA_PHYSICS_H

class Koopa;
class BaseEntity;

class KoopaPhysics {
public:
    void checkObstacles(Koopa& koopa);
    void move(Koopa& koopa, float deltaTime);
    void takeDamage(Koopa& koopa, int damage);
    void onStomped(Koopa& koopa, BaseEntity* attacker);
    void onSideCollision(Koopa& koopa, BaseEntity* attacker);
    void becomeStaticShell(Koopa& koopa);
    void kickShell(Koopa& koopa, int dir); // dir = MoveDirection
};

#endif
