#include "Entities/Character/Enemy/Projectile.h"
#include "Entities/BaseEntity.h"

Projectile::Projectile(float startX, float startY, float velX, float velY, int dmg)
    : position(startX, startY), velocity(velX, velY), isAlive(true), damage(dmg) {
    shape.setPosition(position);
}

void Projectile::onHitPlayer(BaseEntity* player) {
    if (player && isAlive) {
        player->takeDamage(damage);
        die();
    }
}
