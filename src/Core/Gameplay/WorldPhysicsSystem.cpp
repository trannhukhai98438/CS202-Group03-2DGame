#include "Gameplay/WorldPhysicsSystem.h"

#include "Entities/Block/Block.h"
#include "Entities/Character/Enemy/Enemy.h"
#include "Entities/Character/Enemy/Projectile.h"
#include "Entities/Character/Hero/Hero.h"
#include "Entities/Item/Item.h"
#include "Gameplay/GameWorld.h"

#include <algorithm>
#include <cmath>
#include <utility>

void WorldPhysicsSystem::update(GameWorld& world, float deltaTime) {
    resolveHero(world, deltaTime);
    resolveItems(world, deltaTime);
    resolveEnemies(world, deltaTime);
    resolveProjectiles(world, deltaTime);
}

void WorldPhysicsSystem::resolveHero(GameWorld& world, float deltaTime) {
    Hero* hero = world.hero();
    if (!hero || hero->isDead()) return;

    // ── Platform carry ────────────────────────────────────────────────────────
    // When the hero is grounded, the state machine (e.g. IdleState) sets only a
    // small gravity nudge for velocity.y (~GRAVITY*dt ≈ 25 px/s).  A downward
    // Lifter moves at 80 px/s, so the hero cannot keep up through velocity alone
    // and falls behind each frame, causing the grounded flag to flicker.
    //
    // Fix: before capturing oldPosition, pre-move the hero in *position space*
    // by the platform's per-frame displacement.  This bypasses the velocity
    // system entirely, so IdleState's overwrite cannot undo it.
    if (hero->getGrounded()) {
        const sf::FloatRect heroBounds = hero->getBounds();
        const float heroBottom = heroBounds.top + heroBounds.height;
        for (const auto& block : world.blocks()) {
            if (!block->getIsActive() || !block->isSolid()) continue;
            const sf::Vector2f bv = block->getVelocity();
            if (bv.y <= 0.f) continue;   // only carry for downward-moving platforms

            const sf::FloatRect bb = block->getBounds();
            // gap = how far the platform's top has moved below the hero's feet.
            // After block->update() the lifter is already at its new position,
            // so gap == bv.y * deltaTime when the hero was flush last frame.
            // We close exactly this gap (not bv.y*dt again, which would overshoot).
            const float gap = bb.top - heroBottom;
            constexpr float rideEpsilon = 8.f;   // generous enough for low frame-rates
            const bool ridingThisBlock =
                gap > 0.f && gap <= rideEpsilon &&
                heroBounds.left + heroBounds.width > bb.left &&
                heroBounds.left < bb.left + bb.width;

            if (ridingThisBlock) {
                const sf::Vector2f pos = hero->getPosition();
                // Snap hero down by exactly the gap, keeping it flush with the platform.
                hero->setPosition(pos.x, pos.y + gap);
                break;
            }
        }
    }
    // ─────────────────────────────────────────────────────────────────────────

    sf::Vector2f oldPosition = hero->getPosition();
    sf::Vector2f velocity = hero->getVelocity();

    const sf::FloatRect previousXBounds = hero->getBounds();
    hero->setPosition(oldPosition.x + velocity.x * deltaTime, oldPosition.y);
    const sf::FloatRect proposedXBounds = hero->getBounds();

    // A horizontal pass must resolve a wall from horizontal movement, even
    // when the Hero only clips the wall near its top corner. Using the final
    // overlap depth to choose an axis can call that contact Top and skip it,
    // allowing the next Y pass to place the Hero inside a pipe.
    bool hasHorizontalTarget = false;
    float horizontalCollisionEdge = 0.0f;

    if (velocity.x != 0.0f) {
        constexpr float comparisonEpsilon = 0.001f;
        const float previousRight =
            previousXBounds.left + previousXBounds.width;
        const float proposedRight =
            proposedXBounds.left + proposedXBounds.width;

        auto considerWall = [&](const sf::FloatRect& wallBounds) {
            const float verticalOverlap =
                std::min(proposedXBounds.top + proposedXBounds.height,
                         wallBounds.top + wallBounds.height)
                - std::max(proposedXBounds.top, wallBounds.top);
            if (verticalOverlap <= comparisonEpsilon) return;

            if (velocity.x > 0.0f) {
                const bool crossedWallLeft =
                    previousRight <= wallBounds.left + comparisonEpsilon
                    && proposedRight >= wallBounds.left - comparisonEpsilon;
                const bool isCloser = !hasHorizontalTarget
                    || wallBounds.left
                        < horizontalCollisionEdge - comparisonEpsilon;
                if (crossedWallLeft && isCloser) {
                    hasHorizontalTarget = true;
                    horizontalCollisionEdge = wallBounds.left;
                }
                return;
            }

            const float wallRight = wallBounds.left + wallBounds.width;
            const bool crossedWallRight =
                previousXBounds.left >= wallRight - comparisonEpsilon
                && proposedXBounds.left <= wallRight + comparisonEpsilon;
            const bool isCloser = !hasHorizontalTarget
                || wallRight > horizontalCollisionEdge + comparisonEpsilon;
            if (crossedWallRight && isCloser) {
                hasHorizontalTarget = true;
                horizontalCollisionEdge = wallRight;
            }
        };

        for (const auto& collider : world.mapColliders()) {
            considerWall(collider.getGlobalBounds());
        }
        for (const auto& block : world.blocks()) {
            if (block->getIsActive() && block->isSolid()) {
                considerWall(block->getBounds());
            }
        }

        if (hasHorizontalTarget) {
            const float resolvedLeft = velocity.x > 0.0f
                ? horizontalCollisionEdge - proposedXBounds.width
                : horizontalCollisionEdge;
            hero->getHitbox().setPosition(resolvedLeft,
                                          proposedXBounds.top);
            velocity.x = 0.0f;
        }
    }

    // Retain overlap-based recovery for entities that begin a frame already
    // intersecting a solid (for example, after an external position change).
    for (auto& collider : world.mapColliders()) {
        m_physics.resolveCollisionX(hero->getHitbox(), collider, velocity.x);
    }
    for (auto& block : world.blocks()) {
        if (block->getIsActive() && block->isSolid()) {
            m_physics.resolveCollisionX(hero->getHitbox(), block->getHitbox(),
                                        velocity.x);
        }
    }

    // Preserve the X position resolved through the hitbox before moving on Y.
    const float resolvedX = hero->getHitbox().getPosition().x;
    hero->setPosition(resolvedX, oldPosition.y + velocity.y * deltaTime);

    // Select an upward-hit target before any Y collision mutates the Hero's
    // position or velocity. Adjacent blocks can overlap the Hero in the same
    // frame; resolving them in vector order would otherwise let the first one
    // consume the hit even when most of the Hero is below another block.
    Block* upwardBlockTarget = nullptr;
    bool hasUpwardTerrainTarget = false;
    float upwardCollisionBottom = 0.0f;
    float targetHorizontalOverlap = 0.0f;
    float targetCenterDistance = 0.0f;

    const sf::FloatRect proposedBounds = hero->getBounds();
    sf::FloatRect previousBounds = proposedBounds;
    previousBounds.top = oldPosition.y;

    // Resolve landings from the Hero's movement direction, rather than from
    // the smallest final-frame overlap. Near a platform corner, horizontal
    // overlap can be smaller than vertical penetration; the generic AABB side
    // test then reports Left/Right during the Y pass and lets the Hero sink
    // into the surface. Sweeping the feet from their previous to proposed
    // position makes the result deterministic and also prevents tunnelling.
    bool hasDownwardTarget = false;
    float downwardCollisionTop = 0.0f;
    float downwardHorizontalOverlap = 0.0f;
    float downwardCenterDistance = 0.0f;

    if (velocity.y >= 0.0f) {
        constexpr float comparisonEpsilon = 0.001f;
        const float previousBottom =
            previousBounds.top + previousBounds.height;
        const float proposedBottom =
            proposedBounds.top + proposedBounds.height;
        const float heroCenterX =
            proposedBounds.left + proposedBounds.width * 0.5f;

        auto considerLandingSurface = [&](const sf::FloatRect& surfaceBounds) {
            const float horizontalOverlap =
                std::min(proposedBounds.left + proposedBounds.width,
                         surfaceBounds.left + surfaceBounds.width)
                - std::max(proposedBounds.left, surfaceBounds.left);
            const bool crossedSurfaceTop =
                previousBottom <= surfaceBounds.top + comparisonEpsilon
                && proposedBottom >= surfaceBounds.top - comparisonEpsilon;
            if (!crossedSurfaceTop
                || horizontalOverlap <= comparisonEpsilon) {
                return;
            }

            const float centerDistance = std::abs(
                heroCenterX
                - (surfaceBounds.left + surfaceBounds.width * 0.5f));
            const bool isCloserInFallDirection =
                !hasDownwardTarget
                || surfaceBounds.top
                    < downwardCollisionTop - comparisonEpsilon;
            const bool isSameHeight = hasDownwardTarget
                && std::abs(surfaceBounds.top - downwardCollisionTop)
                    <= comparisonEpsilon;
            const bool hasBetterHorizontalMatch = isSameHeight
                && (horizontalOverlap
                        > downwardHorizontalOverlap + comparisonEpsilon
                    || (std::abs(horizontalOverlap
                                 - downwardHorizontalOverlap)
                            <= comparisonEpsilon
                        && centerDistance < downwardCenterDistance));

            if (isCloserInFallDirection || hasBetterHorizontalMatch) {
                hasDownwardTarget = true;
                downwardCollisionTop = surfaceBounds.top;
                downwardHorizontalOverlap = horizontalOverlap;
                downwardCenterDistance = centerDistance;
            }
        };

        for (const auto& collider : world.mapColliders()) {
            considerLandingSurface(collider.getGlobalBounds());
        }
        for (const auto& block : world.blocks()) {
            if (block->getIsActive() && block->isSolid()) {
                considerLandingSurface(block->getBounds());
            }
        }

        if (hasDownwardTarget) {
            hero->getHitbox().setPosition(
                resolvedX, downwardCollisionTop - proposedBounds.height);
            velocity.y = 0.0f;
        }
    }

    if (velocity.y < 0.0f) {
        constexpr float comparisonEpsilon = 0.001f;
        const float heroCenterX = proposedBounds.left
            + proposedBounds.width * 0.5f;

        // Terrain participates in the same swept selection so a block behind
        // a closer ceiling cannot activate through that ceiling.
        for (const auto& collider : world.mapColliders()) {
            const sf::FloatRect colliderBounds = collider.getGlobalBounds();
            const float colliderBottom =
                colliderBounds.top + colliderBounds.height;
            const float horizontalOverlap =
                std::min(proposedBounds.left + proposedBounds.width,
                         colliderBounds.left + colliderBounds.width)
                - std::max(proposedBounds.left, colliderBounds.left);
            const bool crossedColliderBottom =
                previousBounds.top >= colliderBottom
                && proposedBounds.top < colliderBottom;
            if (!crossedColliderBottom || horizontalOverlap <= 0.0f) continue;

            const float centerDistance = std::abs(
                heroCenterX
                - (colliderBounds.left + colliderBounds.width * 0.5f));
            const bool isCloserInJumpDirection =
                !hasUpwardTerrainTarget
                || colliderBottom
                    > upwardCollisionBottom + comparisonEpsilon;
            const bool isSameHeight = hasUpwardTerrainTarget
                && std::abs(colliderBottom - upwardCollisionBottom)
                    <= comparisonEpsilon;
            const bool hasBetterHorizontalMatch = isSameHeight
                && (horizontalOverlap
                        > targetHorizontalOverlap + comparisonEpsilon
                    || (std::abs(horizontalOverlap
                                 - targetHorizontalOverlap)
                            <= comparisonEpsilon
                        && centerDistance < targetCenterDistance));

            if (isCloserInJumpDirection || hasBetterHorizontalMatch) {
                hasUpwardTerrainTarget = true;
                upwardCollisionBottom = colliderBottom;
                targetHorizontalOverlap = horizontalOverlap;
                targetCenterDistance = centerDistance;
            }
        }

        for (auto& block : world.blocks()) {
            if (!block->getIsActive()
                || (!block->isSolid() && !block->canBeHitFromBelow())) {
                continue;
            }

            const sf::FloatRect blockBounds = block->getBounds();
            const float blockBottom = blockBounds.top + blockBounds.height;
            const float horizontalOverlap =
                std::min(proposedBounds.left + proposedBounds.width,
                         blockBounds.left + blockBounds.width)
                - std::max(proposedBounds.left, blockBounds.left);

            const bool crossedBlockBottom =
                previousBounds.top >= blockBottom
                && proposedBounds.top < blockBottom;
            if (!crossedBlockBottom || horizontalOverlap <= 0.0f) continue;

            const float centerDistance = std::abs(
                heroCenterX - (blockBounds.left + blockBounds.width * 0.5f));
            const bool hasSelectedTarget =
                upwardBlockTarget || hasUpwardTerrainTarget;
            const bool isCloserInJumpDirection =
                !hasSelectedTarget
                || blockBottom
                    > upwardCollisionBottom + comparisonEpsilon;
            const bool isSameHeight = hasSelectedTarget
                && std::abs(blockBottom - upwardCollisionBottom)
                    <= comparisonEpsilon;
            const bool hasBetterHorizontalMatch = isSameHeight
                && (horizontalOverlap
                        > targetHorizontalOverlap + comparisonEpsilon
                    || (std::abs(horizontalOverlap
                                 - targetHorizontalOverlap)
                            <= comparisonEpsilon
                        && centerDistance < targetCenterDistance));

            if (isCloserInJumpDirection || hasBetterHorizontalMatch) {
                upwardBlockTarget = block.get();
                upwardCollisionBottom = blockBottom;
                targetHorizontalOverlap = horizontalOverlap;
                targetCenterDistance = centerDistance;
            }
        }
    }

    if (upwardBlockTarget || hasUpwardTerrainTarget) {
        // Directional resolution also handles a fast upward step that crosses
        // an entire underside without ending the frame inside the solid.
        hero->getHitbox().setPosition(resolvedX, upwardCollisionBottom);
        velocity.y = 0.0f;
        if (upwardBlockTarget
            && upwardBlockTarget->canBeHitFromBelow()) {
            if (auto spawnedItem = upwardBlockTarget->hit(hero)) {
                world.addItem(std::move(spawnedItem));
            }
        }
    }

    bool grounded = hasDownwardTarget;
    for (auto& collider : world.mapColliders()) {
        if (m_physics.checkCollision(hero->getHitbox(), collider)
            == SideType::Top) {
            grounded = true;
        }
        m_physics.resolveCollisionY(hero->getHitbox(), collider, velocity.y);
    }
    for (auto& block : world.blocks()) {
        if (!block->getIsActive()) continue;

        const SideType side =
            m_physics.checkCollision(hero->getHitbox(), block->getHitbox());
        const bool isSolid = block->isSolid();

        if (isSolid && side == SideType::Top) {
            grounded = true;
        }

        if (isSolid) {
            m_physics.resolveCollisionY(hero->getHitbox(), block->getHitbox(),
                                        velocity.y);
        }
    }

    hero->setGrounded(grounded);
    const sf::Vector2f resolvedPosition = hero->getHitbox().getPosition();
    hero->setPosition(resolvedPosition.x, resolvedPosition.y);
    hero->setVelocity(velocity);
}

void WorldPhysicsSystem::resolveItems(GameWorld& world, float deltaTime) {
    for (auto& item : world.items()) {
        sf::Vector2f oldPosition = item->getPosition();
        sf::Vector2f velocity = item->getVelocity();

        if (!item->isColliable()) {
            item->setPosition(oldPosition.x + velocity.x * deltaTime,
                              oldPosition.y + velocity.y * deltaTime);
            continue;
        }

        const float oldVelocityX = velocity.x;
        item->setPosition(oldPosition.x + velocity.x * deltaTime,
                          oldPosition.y);
        for (auto& collider : world.mapColliders()) {
            m_physics.resolveCollisionX(item->getHitbox(), collider,
                                        velocity.x);
        }
        for (auto& block : world.blocks()) {
            if (block->getIsActive() && block->isSolid()) {
                m_physics.resolveCollisionX(item->getHitbox(),
                                            block->getHitbox(), velocity.x);
            }
        }

        if (oldVelocityX != 0.0f && velocity.x == 0.0f) {
            item->getCollision();
            velocity = item->getVelocity();
        }

        // Preserve the X position resolved through the hitbox before moving on Y.
        item->setPosition(item->getHitbox().getPosition().x,
                          oldPosition.y + velocity.y * deltaTime);
        bool grounded = false;
        for (auto& collider : world.mapColliders()) {
            if (m_physics.checkCollision(item->getHitbox(), collider)
                == SideType::Top) {
                grounded = true;
            }
            m_physics.resolveCollisionY(item->getHitbox(), collider,
                                        velocity.y);
        }
        for (auto& block : world.blocks()) {
            if (!block->getIsActive() || !block->isSolid()) continue;

            if (m_physics.checkCollision(item->getHitbox(), block->getHitbox())
                == SideType::Top) {
                grounded = true;
            }
            m_physics.resolveCollisionY(item->getHitbox(), block->getHitbox(),
                                        velocity.y);
        }

        item->setGrounded(grounded);
        const sf::Vector2f resolvedPosition = item->getHitbox().getPosition();
        item->setPosition(resolvedPosition.x, resolvedPosition.y);
        item->setVelocity(velocity.x, velocity.y);
    }
}

void WorldPhysicsSystem::resolveEnemies(GameWorld& world, float deltaTime) {
    for (auto& enemy : world.enemies()) {
        if (!enemy->getIsAlive()
            || enemy->getStateName() == "FlippingDeath"
            || enemy->getStateName() == "Squished") {
            continue;
        }

        const sf::Vector2f oldPosition = enemy->getPosition();
        sf::Vector2f velocity = enemy->getVelocity();
        velocity.y += 1500.0f * deltaTime;

        bool hitWall = false;
        for (auto& collider : world.mapColliders()) {
            const SideType side =
                m_physics.checkCollision(enemy->getHitbox(), collider);
            if (side == SideType::Left || side == SideType::Right) {
                hitWall = true;
            }

            float horizontalVelocity =
                static_cast<float>(enemy->getDirection()) * enemy->getSpeed();
            m_physics.resolveCollisionX(enemy->getHitbox(), collider,
                                        horizontalVelocity);
        }
        for (auto& block : world.blocks()) {
            if (!block->getIsActive() || !block->isSolid()) continue;

            const SideType side =
                m_physics.checkCollision(enemy->getHitbox(),
                                         block->getHitbox());
            if (side == SideType::Left || side == SideType::Right) {
                hitWall = true;
            }

            float horizontalVelocity =
                static_cast<float>(enemy->getDirection()) * enemy->getSpeed();
            m_physics.resolveCollisionX(enemy->getHitbox(),
                                        block->getHitbox(),
                                        horizontalVelocity);
        }
        if (hitWall) {
            enemy->flipDirection();
            enemy->notifyWallHit();
        }

        enemy->setPosition(sf::Vector2f(
            enemy->getHitbox().getPosition().x,
            oldPosition.y + velocity.y * deltaTime));
        for (auto& collider : world.mapColliders()) {
            m_physics.resolveCollisionY(enemy->getHitbox(), collider,
                                        velocity.y);
        }
        for (auto& block : world.blocks()) {
            if (!block->getIsActive() || !block->isSolid()) continue;

            m_physics.resolveCollisionY(enemy->getHitbox(),
                                        block->getHitbox(), velocity.y);
        }

        enemy->setVelocity(velocity);
        enemy->setPosition(enemy->getHitbox().getPosition());
    }
}

void WorldPhysicsSystem::resolveProjectiles(GameWorld& world,
                                            float deltaTime) {
    for (auto& projectilePtr : world.projectiles()) {
        Projectile& projectile = *projectilePtr;
        if (!projectile.getIsAlive() || !projectile.usesWorldPhysics()) {
            continue;
        }

        sf::Vector2f velocity = projectile.getVelocity();

        auto forEachSolid = [&](auto&& visitor) {
            for (const auto& collider : world.mapColliders()) {
                if (!visitor(collider.getGlobalBounds())) return;
            }
            for (const auto& block : world.blocks()) {
                if (block->getIsActive() && block->isSolid()
                    && !visitor(block->getHitbox().getGlobalBounds())) {
                    return;
                }
            }
        };

        sf::Vector2f oldPosition = projectile.getPosition();
        projectile.setPosition({oldPosition.x + velocity.x * deltaTime,
                                oldPosition.y});

        forEachSolid([&](const sf::FloatRect& solidBounds) {
            const SideType side =
                m_physics.checkCollision(projectile.getBounds(), solidBounds);
            if (side != SideType::Left && side != SideType::Right) return true;

            m_physics.resolveCollisionX(projectile.getHitbox(), solidBounds,
                                        velocity.x);
            projectile.setPosition(projectile.getHitbox().getPosition());
            projectile.setVelocity(velocity);
            projectile.onSolidCollision(side, solidBounds);
            velocity = projectile.getVelocity();
            return projectile.getIsAlive() && projectile.usesWorldPhysics();
        });

        if (!projectile.getIsAlive() || !projectile.usesWorldPhysics()) {
            continue;
        }

        oldPosition = projectile.getPosition();
        projectile.setPosition({oldPosition.x,
                                oldPosition.y + velocity.y * deltaTime});

        forEachSolid([&](const sf::FloatRect& solidBounds) {
            const SideType side =
                m_physics.checkCollision(projectile.getBounds(), solidBounds);
            if (side == SideType::None) return true;

            if (side == SideType::Top || side == SideType::Bottom) {
                m_physics.resolveCollisionY(projectile.getHitbox(), solidBounds,
                                            velocity.y);
            } else {
                m_physics.resolveCollisionX(projectile.getHitbox(), solidBounds,
                                            velocity.x);
            }
            projectile.setPosition(projectile.getHitbox().getPosition());
            projectile.setVelocity(velocity);
            projectile.onSolidCollision(side, solidBounds);
            velocity = projectile.getVelocity();
            return projectile.getIsAlive() && projectile.usesWorldPhysics();
        });

        projectile.setVelocity(velocity);
    }
}
