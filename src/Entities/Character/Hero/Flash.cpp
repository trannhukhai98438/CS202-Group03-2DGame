#include "Flash.h"

#include "Entities/Projectile/FlashThunder.h"
#include "IdleState.h"
#include "SmallForm.h"
#include "Utilities/ThunderFlashTexture.h"

void Flash::loadTexture(const std::string& path) {
    std::string targetPath = path;
    
    // Override the requested path if we are in Giant form
    if (form && form->getForm() == "Giant" && thunderTextureAvailable) {
        targetPath = specialTexturePath;
    }

	if (targetPath == specialTexturePath) {
		if (thunderTextureAvailable) {
			const sf::Texture* specialTexture = ThunderFlashTexture::get();
			sprite.setTexture(*specialTexture);
			return;
		}
		Hero::loadTexture(baseTexturePath);
		return;
	}

	Hero::loadTexture(targetPath);
}

Flash::Flash(float x, float y, ProjectileSpawnCallback spawnCallback)
	: Hero(x, y, std::move(spawnCallback)) {
	baseTexturePath = "assets/textures/Flash.png";
	specialTexturePath = "assets/textures/thunderflash2.png";
	spriteRenderScale = 0.29f; // Used by Giant form, must match special scale
	thunderTextureAvailable = ThunderFlashTexture::get() != nullptr;
	specialSpriteRenderScale = 0.29f; // Used by Fire form
    smallSpriteRenderScale = 0.166f;

    // Small & Giant Flash (Base Form) uses Flash.png
	const Animation baseIdle({{85, 20, 150, 192}}, 0.15f);
	const Animation baseRun({{347, 20, 160, 192}}, 0.15f);
	const Animation baseSlide({{347, 20, 160, 192}}, 0.15f);
	const Animation baseJump({{629, 20, 130, 192}}, 0.15f);
	const Animation baseSit({{618, 230, 152, 193}}, 0.15f);
	const Animation baseFly({{81, 230, 158, 193}}, 0.15f);
    const Animation baseSpecial({{296, 230, 262, 193}}, 0.15f);

	animator.addAnimation("SmallIdle", baseIdle);
	animator.addAnimation("SmallRun", baseRun);
	animator.addAnimation("SmallSlide", baseSlide);
	animator.addAnimation("SmallJump", baseJump);
	animator.addAnimation("SmallDead", baseSit);
	animator.addAnimation("SmallFly", baseFly);

    // Fire/Thunder Flash uses thunderflash2.png
	const Animation thunderIdle({{50, 20, 149, 220}}, 0.15f);
	const Animation thunderRun({{223, 20, 303, 220}}, 0.15f);
	const Animation thunderSlide({{223, 20, 303, 220}}, 0.15f);
	const Animation thunderJump({{501, 20, 247, 220}}, 0.15f);
	const Animation thunderSit({{524, 280, 201, 228}}, 0.15f);
	const Animation thunderFly({{45, 280, 159, 228}}, 0.15f);
	const Animation thunderSpecial({{204, 280, 341, 228}}, 0.15f);

	if (thunderTextureAvailable) {
        animator.addAnimation("GiantIdle", thunderIdle);
        animator.addAnimation("GiantRun", thunderRun);
        animator.addAnimation("GiantSlide", thunderSlide);
        animator.addAnimation("GiantJump", thunderJump);
        animator.addAnimation("GiantSit", thunderSit);
        animator.addAnimation("GiantFly", thunderFly);

		animator.addAnimation("FireIdle", thunderIdle);
		animator.addAnimation("FireRun", thunderRun);
		animator.addAnimation("FireSlide", thunderSlide);
		animator.addAnimation("FireJump", thunderJump);
		animator.addAnimation("FireSit", thunderSit);
		animator.addAnimation("FireFly", thunderFly);
		animator.addAnimation("FireSpecial", thunderSpecial);
	} else {
        animator.addAnimation("GiantIdle", baseIdle);
        animator.addAnimation("GiantRun", baseRun);
        animator.addAnimation("GiantSlide", baseSlide);
        animator.addAnimation("GiantJump", baseJump);
        animator.addAnimation("GiantSit", baseSit);
        animator.addAnimation("GiantFly", baseFly);

		// A missing optional sheet must not leave Fire animations pointing at
		// unrelated coordinates on the base Flash texture.
		animator.addAnimation("FireIdle", baseIdle);
		animator.addAnimation("FireRun", baseRun);
		animator.addAnimation("FireSlide", baseSlide);
		animator.addAnimation("FireJump", baseJump);
		animator.addAnimation("FireSit", baseSit);
		animator.addAnimation("FireFly", baseFly);
		animator.addAnimation("FireSpecial", baseSpecial);
	}

	animator.addAnimation("SmallGrow", Animation({
		baseIdle.frames[0], thunderIdle.frames[0], baseIdle.frames[0]
	}, 0.15f));
	animator.addAnimation("GiantShrink", Animation({
		thunderIdle.frames[0], baseIdle.frames[0], thunderIdle.frames[0]
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
