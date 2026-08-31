#include "Mario.h"
#include "SmallForm.h"
#include "IdleState.h"
#include "Entities/Projectile/MarioFireball.h"

Mario::Mario(float x, float y, ProjectileSpawnCallback spawnCallback)
    : Hero(x, y, std::move(spawnCallback)) {
    baseTexturePath = "assets/textures/mario.png";
	specialTexturePath = "assets/textures/FireMario.png";
    spriteRenderScale = 0.39f;
    smallSpriteRenderScale = 0.20f;
    specialSpriteRenderScale = 0.39f;
    
    // Small Form
    animator.addAnimation("SmallIdle", Animation({{28, 68, 94, 164}}, 0.15f));
    animator.addAnimation("SmallRun", Animation({{141, 68, 108, 164}, {250, 68, 94, 164}, {344, 60, 118, 172}}, 0.1f));
    animator.addAnimation("SmallSlide", Animation({{478, 68, 114, 165}}, 0.15f));
    animator.addAnimation("SmallJump", Animation({{614, 28, 120, 185}}, 0.15f));
    animator.addAnimation("SmallDead", Animation({{851, 164, 156, 66}}, 0.15f));
    animator.addAnimation("SmallFly", Animation({{614, 28, 120, 185}}, 0.15f));

    // Giant Form
    animator.addAnimation("GiantIdle", Animation({{28, 68, 94, 164}}, 0.15f));
    animator.addAnimation("GiantRun", Animation({{141, 68, 108, 164}, {250, 68, 94, 164}, {344, 60, 118, 172}}, 0.1f));
    animator.addAnimation("GiantSlide", Animation({{478, 68, 114, 165}}, 0.15f));
    animator.addAnimation("GiantJump", Animation({{614, 28, 120, 185}}, 0.15f));
    animator.addAnimation("GiantSit", Animation({{752, 102, 90, 128}}, 0.15f));
    animator.addAnimation("GiantFly", Animation({{614, 28, 120, 185}}, 0.15f));

    // Fire Form
    animator.addAnimation("FireIdle", Animation({{39, 41, 109, 166}}, 0.15f));
    animator.addAnimation("FireRun", Animation({{209, 47, 116, 161}, {353, 47, 114, 161}, {494, 45, 97, 162}}, 0.1f));
    animator.addAnimation("FireSlide", Animation({{494, 45, 97, 162}}, 0.15f));
    animator.addAnimation("FireJump", Animation({{652, 33, 122, 171}}, 0.15f));
    animator.addAnimation("FireSit", Animation({{859, 73, 92, 135}}, 0.15f));
    animator.addAnimation("FireFly", Animation({{652, 33, 122, 171}}, 0.15f));
    animator.addAnimation("FireSpecial", Animation({{769, 233, 90, 159}}, 0.15f));

    // Growth / Shrink
    animator.addAnimation("SmallGrow", Animation({{28, 68, 94, 164}, {752, 102, 90, 128}, {28, 68, 94, 164}}, 0.15f));
    animator.addAnimation("GiantShrink", Animation({{752, 102, 90, 128}, {28, 68, 94, 164}, {752, 102, 90, 128}}, 0.15f));
    setForm(std::make_unique<SmallForm>());
    setState(std::make_unique<IdleState>());
    setPosition(x, y);
}

std::unique_ptr<Projectile> Mario::createSpecialProjectile() const {
    const float direction = facingRight ? 1.0f : -1.0f;
    const float startX = facingRight
        ? position.x + shape.getSize().x + 2.0f
        : position.x - 18.0f;
    const float startY = position.y + shape.getSize().y * 0.55f - 8.0f;
    return std::make_unique<MarioFireball>(startX, startY,
                                           direction * 420.0f);
}
