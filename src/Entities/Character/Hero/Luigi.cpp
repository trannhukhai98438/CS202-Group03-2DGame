#include "Luigi.h"
#include "SmallForm.h"
#include "IdleState.h"
#include "Entities/Projectile/LuigiWaterBomb.h"

Luigi::Luigi(float x, float y, ProjectileSpawnCallback spawnCallback)
    : Hero(x, y, std::move(spawnCallback)) {
    baseTexturePath = "assets/textures/Luigi.png";
	specialTexturePath = "assets/textures/Luigi.png";
    spriteRenderScale = 0.285f;
    smallSpriteRenderScale = 0.142f;
    specialSpriteRenderScale = 0.285f;
    
    // Small Form (Baseline Y = 282)
    animator.addAnimation("SmallIdle", Animation({{27, 56, 118, 227}}, 0.15f));
    animator.addAnimation("SmallRun", Animation({{166, 56, 141, 227}, {329, 56, 145, 227}, {499, 56, 168, 227}}, 0.1f));
    animator.addAnimation("SmallSlide", Animation({{683, 56, 168, 227}}, 0.15f));
    animator.addAnimation("SmallJump", Animation({{860, 47, 141, 236}}, 0.15f));
    animator.addAnimation("SmallDead", Animation({{761, 415, 227, 107}}, 0.15f));
    animator.addAnimation("SmallFly", Animation({{860, 47, 141, 236}}, 0.15f));

    // Giant Form (Baseline Y = 282)
    animator.addAnimation("GiantIdle", Animation({{27, 56, 118, 227}}, 0.15f));
    animator.addAnimation("GiantRun", Animation({{166, 56, 141, 227}, {329, 56, 145, 227}, {499, 56, 168, 227}}, 0.1f));
    animator.addAnimation("GiantSlide", Animation({{683, 56, 168, 227}}, 0.15f));
    animator.addAnimation("GiantJump", Animation({{860, 47, 141, 236}}, 0.15f));
    animator.addAnimation("GiantSit", Animation({{49, 280, 165, 242}}, 0.15f));
    animator.addAnimation("GiantFly", Animation({{860, 47, 141, 236}}, 0.15f));

    // Fire / Special Form
    animator.addAnimation("FireIdle", Animation({{27, 56, 118, 227}}, 0.15f));
    animator.addAnimation("FireRun", Animation({{166, 56, 141, 227}, {329, 56, 145, 227}, {499, 56, 168, 227}}, 0.1f));
    animator.addAnimation("FireSlide", Animation({{683, 56, 168, 227}}, 0.15f));
    animator.addAnimation("FireJump", Animation({{860, 47, 141, 236}}, 0.15f));
    animator.addAnimation("FireSit", Animation({{49, 280, 165, 242}}, 0.15f));
    animator.addAnimation("FireFly", Animation({{860, 47, 141, 236}}, 0.15f));
    animator.addAnimation("FireSpecial", Animation({{459, 321, 152, 199}}, 0.15f));

    // Growth / Shrink
    animator.addAnimation("SmallGrow", Animation({{27, 56, 118, 227}, {49, 280, 165, 242}, {27, 56, 118, 227}}, 0.15f));
    animator.addAnimation("GiantShrink", Animation({{49, 280, 165, 242}, {27, 56, 118, 227}, {49, 280, 165, 242}}, 0.15f));

    setForm(std::make_unique<SmallForm>());
    setState(std::make_unique<IdleState>());
    setPosition(x, y);
}

std::unique_ptr<Projectile> Luigi::createSpecialProjectile() const {
    const float direction = facingRight ? 1.0f : -1.0f;
    const float startX = facingRight
        ? position.x + shape.getSize().x + 2.0f
        : position.x - 26.0f;
    const float startY = position.y + shape.getSize().y * 0.25f;
    return std::make_unique<LuigiWaterBomb>(startX, startY,
                                            direction * 480.0f, -550.0f);
}
