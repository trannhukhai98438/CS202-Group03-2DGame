#ifndef THOR_KING_PHYSICS_H
#define THOR_KING_PHYSICS_H

class ThorKing;
class Character;

class ThorKingPhysics {
public:
    void checkObstacles(ThorKing& boss);
    void move(ThorKing& boss, float deltaTime);
    void onStomped(ThorKing& boss, Character* attacker);
    void onSideCollision(ThorKing& boss, Character* attacker);
    void takeDamage(ThorKing& boss, int damage);
    void notifyWallHit(ThorKing& boss);
};

#endif
