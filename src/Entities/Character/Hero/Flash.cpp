#include "Flash.h"

#include "Entities/Projectile/FlashThunder.h"
#include "IdleState.h"
#include "SmallForm.h"
#include "Utilities/ThunderFlashTexture.h"

void Flash::loadTexture(const std::string& path) {
	if (path == specialTexturePath) {
		if (thunderTextureAvailable) {
			const sf::Texture* specialTexture = ThunderFlashTexture::get();
			sprite.setTexture(*specialTexture);
			return;
		}
		Hero::loadTexture(baseTexturePath);
		return;
	}

	Hero::loadTexture(path);
}

Flash::Flash(float x, float y, ProjectileSpawnCallback spawnCallback)
	: Hero(x, y, std::move(spawnCallback)) {
	baseTexturePath = "assets/textures/Flash.png";
	specialTexturePath = "assets/textures/thunderflash2.png";
	spriteRenderScale = 0.29f; // Used by Giant form
	thunderTextureAvailable = ThunderFlashTexture::get() != nullptr;
	specialSpriteRenderScale = 0.29f; // Used by Fire form (Epic Archmage)
    smallSpriteRenderScale = 0.166f; // Used by Small form (Cute small wizard)

    // Base Flash (Flash.png) - used for Small and Giant forms
	const Animation baseIdle({{85, 20, 150, 192}}, 0.15f);
	const Animation baseRun({{347, 20, 160, 192}}, 0.15f);
	const Animation baseSlide({{347, 20, 160, 192}}, 0.15f);
	const Animation baseJump({{629, 20, 130, 192}}, 0.15f);
	const Animation baseSit({{618, 230, 152, 193}}, 0.15f);
	const Animation baseFly({{81, 230, 158, 193}}, 0.15f);
    const Animation baseSpecial({{296, 230, 262, 193}}, 0.15f);

    // Small Form (Flash.png)
	animator.addAnimation("SmallIdle", baseIdle);
	animator.addAnimation("SmallRun", baseRun);
	animator.addAnimation("SmallSlide", baseSlide);
	animator.addAnimation("SmallJump", baseJump);
	animator.addAnimation("SmallDead", baseSit);
	animator.addAnimation("SmallFly", baseFly);
	animator.addAnimation("SmallCheer", baseFly);

    // Giant Form (Flash.png enlarged)
    animator.addAnimation("GiantIdle", baseIdle);
    animator.addAnimation("GiantRun", baseRun);
    animator.addAnimation("GiantSlide", baseSlide);
    animator.addAnimation("GiantJump", baseJump);
    animator.addAnimation("GiantSit", baseSit);
    animator.addAnimation("GiantFly", baseFly);
	animator.addAnimation("GiantCheer", baseFly);

    // Fire/Thunder Form - Upgraded Stage 2 (thunderflash2.png)
	const Animation thunderIdle({{50, 20, 149, 220}}, 0.15f);
	const Animation thunderRun({{223, 20, 303, 220}}, 0.15f);
	const Animation thunderSlide({{223, 20, 303, 220}}, 0.15f);
	const Animation thunderJump({{501, 20, 247, 220}}, 0.15f);
	const Animation thunderSit({{524, 280, 201, 228}}, 0.15f);
	const Animation thunderFly({{45, 280, 159, 228}}, 0.15f);
	const Animation thunderSpecial({{204, 280, 341, 228}}, 0.15f);
	if (thunderTextureAvailable) {
		animator.addAnimation("FireIdle", thunderIdle);
		animator.addAnimation("FireRun", thunderRun);
		animator.addAnimation("FireSlide", thunderSlide);
		animator.addAnimation("FireJump", thunderJump);
		animator.addAnimation("FireSit", thunderSit);
		animator.addAnimation("FireCheer", thunderFly);
		animator.addAnimation("FireFly", thunderFly);
		animator.addAnimation("FireSpecial", thunderSpecial);
	} else {
		// Fallback to base texture coordinates if optional sheet missing
		animator.addAnimation("FireIdle", baseIdle);
		animator.addAnimation("FireRun", baseRun);
		animator.addAnimation("FireSlide", baseSlide);
		animator.addAnimation("FireJump", baseJump);
		animator.addAnimation("FireSit", baseSit);
		animator.addAnimation("FireFly", baseFly);
		animator.addAnimation("FireCheer", baseFly);
		animator.addAnimation("FireSpecial", baseSpecial);
	}

	animator.addAnimation("SmallGrow", Animation({
		baseIdle.frames[0], baseSit.frames[0], baseIdle.frames[0]
	}, 0.15f));
	animator.addAnimation("GiantShrink", Animation({
		baseSit.frames[0], baseIdle.frames[0], baseSit.frames[0]
	}, 0.15f));

	setForm(std::make_unique<SmallForm>());
	setState(std::make_unique<IdleState>());
	setPosition(x, y);
}

std::unique_ptr<Projectile> Flash::createSpecialProjectile() const {
	const float direction = facingRight ? 1.f : -1.f;
	const float startX = facingRight
		? position.x + shape.getSize().x + 2.f : position.x - 64.f;
	const float startY = position.y + shape.getSize().y * 0.35f;
	return std::make_unique<FlashThunder>(startX, startY, direction * 650.f);
}
