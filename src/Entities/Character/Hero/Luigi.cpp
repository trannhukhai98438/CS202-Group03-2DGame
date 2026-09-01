#include "Luigi.h"
#include "SmallForm.h"
#include "IdleState.h"
#include "Entities/Projectile/LuigiWaterBomb.h"

Luigi::Luigi(float x, float y, ProjectileSpawnCallback spawnCallback)
    : Hero(x, y, std::move(spawnCallback)) {
    baseTexturePath = "assets/textures/Luigi.png";
	specialTexturePath = "assets/textures/KitsuneLuigi.png";
    spriteRenderScale = 0.285f;
    smallSpriteRenderScale = 0.142f;
    specialSpriteRenderScale = 0.285f;
    
    // Small Form (Base Luigi.png)
    animator.addAnimation("SmallIdle", Animation({{27, 56, 118, 227}}, 0.15f));
    animator.addAnimation("SmallRun", Animation({{166, 56, 141, 227}, {329, 56, 145, 227}, {499, 56, 168, 227}}, 0.1f));
    animator.addAnimation("SmallSlide", Animation({{683, 56, 168, 227}}, 0.15f));
    animator.addAnimation("SmallJump", Animation({{860, 47, 141, 236}}, 0.15f));
    animator.addAnimation("SmallDead", Animation({{761, 415, 227, 107}}, 0.15f));
    animator.addAnimation("SmallFly", Animation({{860, 47, 141, 236}}, 0.15f));

    // Giant Form (Enlarged Base Luigi.png)
    animator.addAnimation("GiantIdle", Animation({{27, 56, 118, 227}}, 0.15f));
    animator.addAnimation("GiantRun", Animation({{166, 56, 141, 227}, {329, 56, 145, 227}, {499, 56, 168, 227}}, 0.1f));
    animator.addAnimation("GiantSlide", Animation({{683, 56, 168, 227}}, 0.15f));
    animator.addAnimation("GiantJump", Animation({{860, 47, 141, 236}}, 0.15f));
    animator.addAnimation("GiantSit", Animation({{49, 280, 165, 242}}, 0.15f));
    animator.addAnimation("GiantFly", Animation({{860, 47, 141, 236}}, 0.15f));

    // Fire Form (Kitsune Fox Luigi - Stage 2 Upgraded form from KitsuneLuigi.png)
    animator.addAnimation("FireIdle", Animation({{19, 39, 171, 221}}, 0.15f));
    animator.addAnimation("FireRun", Animation({
        {202, 39, 161, 221}, // Step 1: Left foot forward
        {395, 36, 198, 221}, // Step 2: Passing center
        {608, 36, 198, 223}  // Step 3: Right foot forward
    }, 0.10f));
    animator.addAnimation("FireSlide", Animation({{608, 36, 198, 223}}, 0.15f));
    animator.addAnimation("FireJump", Animation({{839, 18, 152, 226}}, 0.15f));
    animator.addAnimation("FireSit", Animation({{30, 362, 158, 181}}, 0.15f));
    animator.addAnimation("FireFly", Animation({{234, 330, 237, 156}}, 0.15f));
    animator.addAnimation("FireSpecial", Animation({
        {514, 325, 279, 217}, // Pose 8: 360-degree green wind tail spin
        {820, 294, 177, 250}  // Pose 9: Water magic cast / throw
    }, 0.15f));

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
