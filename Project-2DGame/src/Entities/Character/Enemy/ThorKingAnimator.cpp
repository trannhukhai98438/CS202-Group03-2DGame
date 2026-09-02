#include "Entities/Character/Enemy/ThorKingAnimator.h"
#include "Entities/Character/Enemy/ThorKing.h"
#include <cmath>

void ThorKingAnimator::initAnimations(ThorKing& boss) {
    if (!m_loaded) {
        m_texNormal.loadFromFile("assets/textures/boss.png");
        m_texEnraged.loadFromFile("assets/textures/boss_enraged.png");
        m_texSkyMeteor.loadFromFile("assets/textures/boss_sky_meteor.png");
        m_texWinged.loadFromFile("assets/textures/boss_winged.png");
        m_loaded = true;
    }

    boss.getSprite().setTexture(m_texNormal);
    boss.setSpriteOffsetY(6.0f);

    sf::Sprite& sprite = boss.getSprite();
    float scale = 128.f / 132.f;
    sprite.setScale(scale, scale);

    auto& animator = boss.getAnimator();

    // ==========================================
    // Phase 1 (Base Spritesheet: boss.png)
    // ==========================================
    // Walk (6 frames): Baseline Y = 140, H = 132
    animator.addAnimation("walk", Animation({
        sf::IntRect(14, 9, 161, 132),
        sf::IntRect(185, 9, 158, 132),
        sf::IntRect(347, 9, 166, 132),
        sf::IntRect(521, 9, 158, 132),
        sf::IntRect(688, 9, 158, 132),
        sf::IntRect(851, 9, 161, 132)
    }, 0.12f));

    // Crouch (2 frames): Baseline Y = 284, H = 116
    animator.addAnimation("crouch", Animation({
        sf::IntRect(16, 168, 159, 116),
        sf::IntRect(187, 168, 158, 116)
    }, 0.15f));

    // Roll (3 frames): Baseline Y = 284, H = 116
    animator.addAnimation("roll", Animation({
        sf::IntRect(380, 168, 112, 116),
        sf::IntRect(546, 168, 108, 116),
        sf::IntRect(707, 168, 108, 116)
    }, 0.06f));

    // StandUp (1 frame): Baseline Y = 284, H = 116
    animator.addAnimation("standup", Animation({
        sf::IntRect(854, 168, 155, 116)
    }, 0.2f));

    // Fire Breath (4 frames): Baseline Y = 428, H = 133
    animator.addAnimation("fire", Animation({
        sf::IntRect(16, 296, 166, 133),
        sf::IntRect(202, 296, 167, 133),
        sf::IntRect(385, 296, 167, 133),
        sf::IntRect(565, 296, 170, 133)
    }, 0.15f));

    // Stunned (2 frames): Baseline Y = 569, H = 115
    animator.addAnimation("stunned", Animation({
        sf::IntRect(17, 455, 177, 115),
        sf::IntRect(228, 455, 167, 115)
    }, 0.18f));

    // Death (3 frames): Baseline Y = 569, H = 115
    animator.addAnimation("death", Animation({
        sf::IntRect(425, 455, 168, 115),
        sf::IntRect(626, 455, 174, 115),
        sf::IntRect(823, 455, 195, 115)
    }, 0.22f));

    // ==========================================
    // Phase 2 (Enraged Spritesheet: boss_enraged.png)
    // ==========================================
    // Enraged Ground Stomp Walk (6 frames): Baseline Y = 140, H = 136
    animator.addAnimation("walk_enraged", Animation({
        sf::IntRect(14, 4, 161, 136),
        sf::IntRect(184, 4, 156, 136),
        sf::IntRect(356, 4, 156, 136),
        sf::IntRect(521, 4, 158, 136),
        sf::IntRect(687, 4, 160, 136),
        sf::IntRect(853, 4, 159, 136)
    }, 0.09f));

    // Enraged Flaming Crouch (2 frames): Baseline Y = 284, H = 128
    animator.addAnimation("crouch_enraged", Animation({
        sf::IntRect(16, 156, 159, 128),
        sf::IntRect(216, 156, 117, 128)
    }, 0.10f));

    // Enraged Supercharged Flaming Roll (3 frames): Baseline Y = 284, H = 128
    animator.addAnimation("roll_enraged", Animation({
        sf::IntRect(355, 156, 138, 128),
        sf::IntRect(532, 156, 129, 128),
        sf::IntRect(705, 156, 115, 128)
    }, 0.04f));

    // Enraged Fire Breath (2 frames): Baseline Y = 428, H = 139
    animator.addAnimation("fire_enraged", Animation({
        sf::IntRect(16, 289, 166, 139),
        sf::IntRect(202, 289, 167, 139)
    }, 0.10f));

    // Enraged Roar / Powerup Transformation (5 frames): Baseline Y = 570, H = 138
    animator.addAnimation("roar_enraged", Animation({
        sf::IntRect(20, 432, 162, 138),
        sf::IntRect(221, 432, 161, 138),
        sf::IntRect(421, 432, 168, 138),
        sf::IntRect(623, 432, 173, 138),
        sf::IntRect(832, 432, 169, 138)
    }, 0.14f));

    // ==========================================
    // Phase 3 (Epic Winged Demon & Sky Meteor Launch)
    // ==========================================
    // Phase 3: Winged Dragon Walk (5 frames): Baseline Y = 570, H = 136
    animator.addAnimation("walk_winged", Animation({
        sf::IntRect(4, 435, 181, 136),
        sf::IntRect(203, 435, 183, 136),
        sf::IntRect(417, 435, 184, 136),
        sf::IntRect(615, 435, 185, 136),
        sf::IntRect(817, 435, 184, 136)
    }, 0.08f));

    // Phase 3: Upward Sky Meteor Blast (10 frames total, looping fire column for pulsation)
    // Frame 0-2: windup. Frame 3-8: fire column pulsating (loops frames 3 and 4). Frame 9: cooldown.
    animator.addAnimation("sky_meteor", Animation({
        sf::IntRect(24, 275, 183, 227),    // Frame 0: ready/windup
        sf::IntRect(242, 262, 214, 240),   // Frame 1: tilt 45
        sf::IntRect(469, 239, 214, 263),   // Frame 2: tilt 90
        sf::IntRect(693, 48, 222, 454),    // Frame 3: fire blast start
        sf::IntRect(916, 6, 229, 496),     // Frame 4: max fire column
        sf::IntRect(693, 48, 222, 454),    // Frame 5: pulsating fire column
        sf::IntRect(916, 6, 229, 496),     // Frame 6: pulsating fire column
        sf::IntRect(693, 48, 222, 454),    // Frame 7: pulsating fire column
        sf::IntRect(916, 6, 229, 496),     // Frame 8: pulsating fire column
        sf::IntRect(1145, 108, 220, 394)   // Frame 9: cooldown/smoke
    }, 0.12f));

    // Phase 3: Winged Horizontal Fire Breath (4 frames): Y = 289, H = 139 (from boss_winged.png)
    animator.addAnimation("fire_winged", Animation({
        sf::IntRect(16, 289, 166, 139),
        sf::IntRect(202, 289, 167, 139),
        sf::IntRect(385, 289, 167, 139),
        sf::IntRect(565, 289, 170, 139)
    }, 0.15f));

    // Phase 3: Ultra Blazing Roll (3 frames): Baseline Y = 284, H = 128
    animator.addAnimation("roll_berserk", Animation({
        sf::IntRect(355, 156, 138, 128),
        sf::IntRect(532, 156, 129, 128),
        sf::IntRect(705, 156, 115, 128)
    }, 0.025f));
}

void ThorKingAnimator::applyAnimation(ThorKing& boss) {
    std::string state = boss.getStateName();
    int phase = boss.getPhase();
    sf::Sprite& sprite = boss.getSprite();

    // Scale dynamically per spritesheet to guarantee exact height alignment
    float baseScale = 128.f / 132.f;
    if (phase == 3) {
        if (state == "TKFire") {
            if (boss.getShotSeq() % 2 == 1) {
                baseScale = 128.f / 139.f; // Horizontal winged magma shot height (139px)
            } else {
                baseScale = 128.f / 227.f; // Sky meteor column height (227px)
            }
        } else {
            baseScale = 128.f / 136.f;
        }
    } else if (phase >= 2 && state != "TKStunned" && state != "FlippingDeath") {
        baseScale = 128.f / 136.f;
    }

    if (boss.getDirection() == MoveDirection::Right) {
        sprite.setScale(baseScale, baseScale);
    } else {
        sprite.setScale(-baseScale, baseScale);
    }

    if (state == "TKRoar") {
        sprite.setTexture(m_texEnraged);
        sprite.setColor(sf::Color(255, 200, 200));
        boss.getAnimator().playAnimation("roar_enraged", 0.016f);
        return;
    }

    if (phase == 1) {
        sprite.setTexture(m_texNormal);
        sprite.setColor(sf::Color::White);
        if (state == "TKPatrol") {
            boss.getAnimator().playAnimation("walk", 0.016f);
        } else if (state == "TKCrouch") {
            boss.getAnimator().playAnimation("crouch", 0.016f);
        } else if (state == "TKRolling") {
            boss.getAnimator().playAnimation("roll", 0.016f);
        } else if (state == "TKStunned") {
            boss.getAnimator().playAnimation("stunned", 0.016f);
        } else if (state == "TKFire") {
            boss.getAnimator().playAnimation("fire", 0.016f);
        } else if (state == "FlippingDeath") {
            boss.getAnimator().playAnimation("death", 0.016f);
        } else {
            boss.getAnimator().playAnimation("walk", 0.016f);
        }
    } else if (phase == 2) {
        // Phase 2: Enraged Amber Fire Form
        if (state == "TKStunned") {
            sprite.setTexture(m_texNormal);
            sprite.setColor(sf::Color::White);
            boss.getAnimator().playAnimation("stunned", 0.016f);
        } else if (state == "FlippingDeath") {
            sprite.setTexture(m_texNormal);
            sprite.setColor(sf::Color::White);
            boss.getAnimator().playAnimation("death", 0.016f);
        } else {
            sprite.setTexture(m_texEnraged);
            sprite.setColor(sf::Color(255, 240, 220));
            if (state == "TKPatrol") {
                boss.getAnimator().playAnimation("walk_enraged", 0.016f);
            } else if (state == "TKCrouch") {
                boss.getAnimator().playAnimation("crouch_enraged", 0.016f);
            } else if (state == "TKRolling") {
                boss.getAnimator().playAnimation("roll_enraged", 0.016f);
            } else if (state == "TKFire") {
                boss.getAnimator().playAnimation("fire_enraged", 0.016f);
            } else {
                boss.getAnimator().playAnimation("walk_enraged", 0.016f);
            }
        }
    } else {
        // Phase 3: Ultimate Winged Magma Demon Form & Sky Meteor Blast
        if (state == "TKStunned") {
            sprite.setTexture(m_texNormal);
            sprite.setColor(sf::Color(255, 180, 180));
            boss.getAnimator().playAnimation("stunned", 0.016f);
        } else if (state == "FlippingDeath") {
            sprite.setTexture(m_texNormal);
            sprite.setColor(sf::Color::White);
            boss.getAnimator().playAnimation("death", 0.016f);
        } else if (state == "TKFire") {
            if (boss.getShotSeq() % 2 == 1) {
                sprite.setTexture(m_texWinged);
                sprite.setColor(sf::Color(255, 110, 110)); // Crimson Magma glow
                boss.getAnimator().playAnimation("fire_winged", 0.016f);
            } else {
                sprite.setTexture(m_texSkyMeteor);
                sprite.setColor(sf::Color(255, 230, 230));
                boss.getAnimator().playAnimation("sky_meteor", 0.016f);
            }
        } else if (state == "TKPatrol") {
            sprite.setTexture(m_texWinged);
            sprite.setColor(sf::Color(255, 220, 220));
            boss.getAnimator().playAnimation("walk_winged", 0.016f);
        } else {
            sprite.setTexture(m_texEnraged);
            sprite.setColor(sf::Color(255, 110, 110)); // Crimson Magma glow
            if (state == "TKCrouch") {
                boss.getAnimator().playAnimation("crouch_enraged", 0.016f);
            } else if (state == "TKRolling") {
                boss.getAnimator().playAnimation("roll_berserk", 0.016f);
            } else {
                boss.getAnimator().playAnimation("walk_winged", 0.016f);
            }
        }
    }
}
