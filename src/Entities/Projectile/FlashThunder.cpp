#include "Entities/Projectile/FlashThunder.h"
#include "Entities/Character/Character.h"
#include "Utilities/ThunderFlashTexture.h"

#include <iostream>

FlashThunder::FlashThunder(float startX, float startY, float velocityX)
	: Projectile(startX, startY, velocityX, 0.f,
	             ProjectileFaction::Hero, 1) {
	shape.setSize({32.f, 32.f});
	shape.setFillColor(sf::Color(165, 70, 255));
	
	// Since thunderflash2.png lacks a clear projectile sprite, we use the fireball texture
	// from FireMario and tint it purple to look like a ball of lightning/plasma.
	static sf::Texture staticTex;
	static bool loaded = false;
	if (!loaded) {
		if (staticTex.loadFromFile("assets/textures/FireMario.png")) {
			staticTex.setSmooth(true);
			loaded = true;
		}
	}
	
	if (loaded) {
		const sf::IntRect flyingFrame{250, 446, 91, 75};
		const float renderScale = 40.f / flyingFrame.width;
		sprite.setTexture(staticTex);
		sprite.setTextureRect(flyingFrame);
		sprite.setOrigin(flyingFrame.width * 0.5f,
		                 flyingFrame.height * 0.5f);
		sprite.setScale(velocityX < 0.f ? -renderScale : renderScale,
		                renderScale);
		// Tint it bright purple/white for lightning
		sprite.setColor(sf::Color(220, 150, 255));
	}
	setPosition({startX, startY});
}

void FlashThunder::update(float deltaTime) {
	if (!isAlive) return;
	if (phase == Phase::Flying) {
		lifetime -= deltaTime;
		if (lifetime <= 0.f) enterImpact();
		return;
	}

	frameTimer += deltaTime;
	constexpr float FRAME_DURATION = 0.09f;
	while (frameTimer >= FRAME_DURATION && isAlive) {
		frameTimer -= FRAME_DURATION;
		++impactFrame;
		if (impactFrame >= impactFrames.size()) {
			die();
		} else {
			applyImpactFrame();
		}
	}
}

void FlashThunder::render(sf::RenderWindow& window) {
	if (!isAlive) return;
	if (sprite.getTexture()) window.draw(sprite);
	else window.draw(shape);
}

void FlashThunder::onSolidCollision(SideType, const sf::FloatRect&) {
	enterImpact();
}

bool FlashThunder::onHitTarget(Character& target) {
	if (!isAlive || !target.getIsAlive()) return false;
	if (phase == Phase::Impact && !damageWindowOpen) return false;
	if (hitTargets.find(&target) != hitTargets.end()) return false;
	if (phase == Phase::Flying) enterImpact();

	hitTargets.insert(&target);
	target.takeDamage(damage);
	return true;
}

void FlashThunder::onTargetResolutionComplete() {
	if (phase == Phase::Impact) damageWindowOpen = false;
}

void FlashThunder::setPosition(const sf::Vector2f& pos) {
	Projectile::setPosition(pos);
	sprite.setPosition(position.x + shape.getSize().x * 0.5f,
	                   position.y + shape.getSize().y * 0.5f);
}

void FlashThunder::enterImpact() {
	if (phase == Phase::Impact) return;
	const sf::Vector2f center(position.x + shape.getSize().x * 0.5f,
	                         position.y + shape.getSize().y * 0.5f);
	phase = Phase::Impact;
	velocity = {0.f, 0.f};
	damageWindowOpen = true;
	impactFrame = 0;
	frameTimer = 0.f;
	shape.setSize({112.f, 112.f});
	setPosition({center.x - 56.f, center.y - 56.f});
	applyImpactFrame();
}

void FlashThunder::applyImpactFrame() {
	// The texture is already loaded into sprite by the constructor.
    // If not loaded, we just return.
    if (!sprite.getTexture() || impactFrame >= impactFrames.size()) return;
	
    // We use the Mario Fireball impact frames since it's the same texture now.
    // We tint it purple to match.
    const std::array<sf::IntRect, 4> customImpactFrames{{
        {181, 608, 116, 111}, {360, 605, 128, 126},
        {523, 606, 124, 123}, {708, 626, 85, 87}
    }};

	const sf::IntRect& frame = customImpactFrames[impactFrame];
	sprite.setTextureRect(frame);
	sprite.setOrigin(frame.width * 0.5f, frame.height * 0.5f);
	// One common scale preserves the sheet's intended progression from a
	// small spark into the full-size final burst.
	const float renderScale = 112.f / 128.f;
	sprite.setScale(renderScale, renderScale);
    sprite.setColor(sf::Color(220, 150, 255));
	sprite.setPosition(position.x + shape.getSize().x * 0.5f,
	                   position.y + shape.getSize().y * 0.5f);
}
