#include "Entities/Character/Enemy/Projectile.h"
#include "Entities/Character/Character.h"

Projectile::Projectile(float startX, float startY, float velX, float velY,
                       ProjectileFaction owner, int dmg)
    : position(startX, startY), velocity(velX, velY), isAlive(true),
      damage(dmg), faction(owner) {
    shape.setPosition(position);
}

bool Projectile::onHitTarget(Character& target) {
    if (!isAlive || !target.getIsAlive()) return false;
    target.takeDamage(damage);
    die();
    return true;
}
