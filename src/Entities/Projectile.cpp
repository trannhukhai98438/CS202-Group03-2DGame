#include "Entities/Projectile.h"
#include "Entities/Character.h"

Projectile::Projectile(float startX, float startY, float velX, float velY, int dmg)
    : position(startX, startY), velocity(velX, velY), isAlive(true), damage(dmg) {
    shape.setPosition(position);
}

void Projectile::onHitPlayer(Character* player) {
    if (player && isAlive) {
        player->takeDamage(damage);
        die();
    }
}
