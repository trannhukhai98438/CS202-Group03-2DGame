#include "Mario.h"
#include "SmallForm.h"
#include "IdleState.h"
#include "Entities/Projectile/MarioFireball.h"

Mario::Mario(float x, float y, ProjectileSpawnCallback spawnCallback)
    : Hero(x, y, std::move(spawnCallback)) {
    baseTexturePath = "assets/textures/mario.png";
	specialTexturePath = "assets/textures/FireMario.png";
    
    // Small Form
    animator.addAnimation("SmallIdle", Animation({{0, 8, 16, 16}}, 0.15f));
    animator.addAnimation("SmallRun", Animation({{20, 8, 16, 16}, {38, 8, 16, 16}, {56, 8, 16, 16}}, 0.1f));
    animator.addAnimation("SmallSlide", Animation({{76, 8, 16, 16}}, 0.15f));
    animator.addAnimation("SmallJump", Animation({{96, 8, 16, 16}}, 0.15f));
    animator.addAnimation("SmallDead", Animation({{116, 8, 16, 16}}, 0.15f));
    animator.addAnimation("SmallFly", Animation({{136, 8, 16, 16}, {154, 8, 16, 16}, {174, 8, 16, 16}, {192, 8, 16, 16}, {210, 8, 16, 16}, {228, 8, 16, 16}, {246, 8, 16, 16}}, 0.1f));

    // Giant Form
    animator.addAnimation("GiantIdle", Animation({{0, 32, 16, 32}}, 0.15f));
    animator.addAnimation("GiantRun", Animation({{20, 32, 16, 32}, {38, 32, 16, 32}, {56, 32, 16, 32}}, 0.1f));
    animator.addAnimation("GiantSlide", Animation({{76, 32, 16, 32}}, 0.15f));
    animator.addAnimation("GiantJump", Animation({{96, 32, 16, 32}}, 0.15f));
    animator.addAnimation("GiantSit", Animation({{116, 40, 16, 24}}, 0.15f));
    animator.addAnimation("GiantFly", Animation({{136, 31, 16, 32}, {154, 31, 16, 32}, {174, 31, 16, 32}, {192, 31, 16, 32}, {210, 31, 16, 32}, {228, 31, 16, 32}, {246, 31, 16, 32}, {264, 31, 16, 32}}, 0.1f));

    // Fire Form (uses Giant coords + Fire Special)
    animator.addAnimation("FireIdle", Animation({{0, 32, 16, 32}}, 0.15f));
    animator.addAnimation("FireRun", Animation({{20, 32, 16, 32}, {38, 32, 16, 32}, {56, 32, 16, 32}}, 0.1f));
    animator.addAnimation("FireSlide", Animation({{76, 32, 16, 32}}, 0.15f));
    animator.addAnimation("FireJump", Animation({{96, 32, 16, 32}}, 0.15f));
    animator.addAnimation("FireSit", Animation({{116, 40, 16, 24}}, 0.15f));
    animator.addAnimation("FireFly", Animation({{136, 31, 16, 32}, {154, 31, 16, 32}, {174, 31, 16, 32}, {192, 31, 16, 32}, {210, 31, 16, 32}, {228, 31, 16, 32}, {246, 31, 16, 32}, {264, 31, 16, 32}}, 0.1f));
    animator.addAnimation("FireSpecial", Animation({{136, 72, 16, 32}, {154, 72, 16, 32}}, 0.15f));

    // Growth / Shrink
    animator.addAnimation("SmallGrow", Animation({{0, 88, 16, 16}, {18, 80, 16, 24}, {36, 72, 16, 32}}, 0.15f));
    animator.addAnimation("GiantShrink", Animation({{56, 72, 16, 32}, {74, 72, 16, 32}, {92, 88, 16, 16}, {110, 88, 16, 16}}, 0.15f));
    setForm(std::make_unique<SmallForm>());
    setState(std::make_unique<IdleState>());
    sf::Vector2f size=shape.getSize();
    sprite.setOrigin(size.x/2.0f,size.y);
    setPosition(x,y);
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
