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
	spriteRenderScale = 0.29f;
	// Preprocess once during level construction, not on the first flower or
	// thunder cast, and share the result with every projectile.
	thunderTextureAvailable = ThunderFlashTexture::get() != nullptr;
	specialSpriteRenderScale = thunderTextureAvailable
		? 0.33f : spriteRenderScale;

	// Small Flash row. Rectangles deliberately exclude the sheet headings.
	animator.addAnimation("SmallIdle", Animation({{15, 52, 104, 113}}, 0.15f));
	animator.addAnimation("SmallRun", Animation({
		{131, 51, 108, 111}, {253, 51, 96, 111}, {369, 51, 99, 111}
	}, 0.10f));
	animator.addAnimation("SmallSlide", Animation({{489, 51, 82, 111}}, 0.15f));
	animator.addAnimation("SmallJump", Animation({{605, 51, 106, 111}}, 0.15f));
	animator.addAnimation("SmallDead", Animation({{723, 51, 127, 111}}, 0.15f));
	animator.addAnimation("SmallFly", Animation({
		{883, 51, 107, 111}, {1023, 49, 111, 113},
		{1148, 50, 98, 112}, {1275, 52, 123, 110}
	}, 0.10f));

	// Big Flash remains on the base sheet.
	const Animation bigIdle({{0, 221, 125, 208}}, 0.15f);
	const Animation bigRun({
		{130, 214, 125, 215}, {258, 211, 150, 218}, {402, 215, 139, 214}
	}, 0.10f);
	const Animation bigSlide({{549, 200, 123, 228}}, 0.15f);
	const Animation bigJump({{672, 218, 139, 216}}, 0.15f);
	const Animation bigSit({{815, 267, 148, 164}}, 0.15f);
	const Animation bigFly({
		{959, 209, 137, 221}, {1097, 222, 139, 208},
		{1234, 226, 143, 203}, {1375, 219, 141, 210}
	}, 0.10f);
	// Big Thunder Flash row from thunderflash2.png. Every frame uses y=430 as
	// its source baseline, so movement does not make the feet jitter.
	const Animation thunderIdle({{16, 244, 129, 186}}, 0.15f);
	const Animation thunderRun({
		{151, 243, 139, 187}, {291, 245, 157, 185},
		{453, 246, 125, 184}
	}, 0.10f);
	const Animation thunderSlide({{587, 240, 119, 190}}, 0.15f);
	const Animation thunderJump({{709, 244, 136, 186}}, 0.15f);
	const Animation thunderSit({{853, 283, 132, 147}}, 0.15f);
	const Animation thunderFly({
		{985, 237, 122, 193}, {1110, 248, 132, 182},
		{1246, 249, 137, 181}, {1387, 247, 132, 183}
	}, 0.10f);
	const Animation thunderSpecial({
		{1525, 248, 130, 182}, {1655, 246, 121, 184},
		{1777, 248, 145, 182}
	}, 0.10f);

	animator.addAnimation("GiantIdle", bigIdle);
	animator.addAnimation("GiantRun", bigRun);
	animator.addAnimation("GiantSlide", bigSlide);
	animator.addAnimation("GiantJump", bigJump);
	animator.addAnimation("GiantSit", bigSit);
	animator.addAnimation("GiantFly", bigFly);
	if (thunderTextureAvailable) {
		animator.addAnimation("FireIdle", thunderIdle);
		animator.addAnimation("FireRun", thunderRun);
		animator.addAnimation("FireSlide", thunderSlide);
		animator.addAnimation("FireJump", thunderJump);
		animator.addAnimation("FireSit", thunderSit);
		animator.addAnimation("FireFly", thunderFly);
		animator.addAnimation("FireSpecial", thunderSpecial);
	} else {
		// A missing optional sheet must not leave Fire animations pointing at
		// unrelated coordinates on the base Flash texture.
		animator.addAnimation("FireIdle", bigIdle);
		animator.addAnimation("FireRun", bigRun);
		animator.addAnimation("FireSlide", bigSlide);
		animator.addAnimation("FireJump", bigJump);
		animator.addAnimation("FireSit", bigSit);
		animator.addAnimation("FireFly", bigFly);
		animator.addAnimation("FireSpecial", bigSit);
	}

	animator.addAnimation("SmallGrow", Animation({
		{14, 600, 110, 128}, {128, 554, 127, 173}, {253, 511, 134, 214}
	}, 0.15f));
	animator.addAnimation("GiantShrink", Animation({
		{345, 500, 140, 228}, {253, 511, 134, 214},
		{128, 554, 127, 173}, {14, 600, 110, 128}
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
