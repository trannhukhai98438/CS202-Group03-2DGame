#include "Entities/Projectile/FlashThunder.h"
#include "Entities/Character/Character.h"
#include "Utilities/ThunderFlashTexture.h"

namespace {
const sf::IntRect THUNDER_STRIKE_FRAME{772, 246, 210, 76};
constexpr float STRIKE_RENDER_WIDTH = 96.f;
constexpr std::size_t IMPACT_FRAME_COUNT = 4;
}

FlashThunder::FlashThunder(float startX, float startY, float velocityX)
	: Projectile(startX, startY, velocityX, 0.f,
	             ProjectileFaction::Hero, 1),
	  travelsRight(velocityX >= 0.f) {
	shape.setSize({64.f, 24.f});
	shape.setFillColor(sf::Color(165, 70, 255));

	if (const sf::Texture* thunderTexture = ThunderFlashTexture::get()) {
		const float renderScale =
			STRIKE_RENDER_WIDTH / THUNDER_STRIKE_FRAME.width;
		sprite.setTexture(*thunderTexture);
		sprite.setTextureRect(THUNDER_STRIKE_FRAME);
		sprite.setOrigin(THUNDER_STRIKE_FRAME.width * 0.5f,
		                 THUNDER_STRIKE_FRAME.height * 0.5f);
		sprite.setScale(travelsRight ? renderScale : -renderScale,
		                renderScale);
		sprite.setColor(sf::Color::White);
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
		if (impactFrame >= IMPACT_FRAME_COUNT) {
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
	shape.setSize({120.f, 64.f});
	setPosition({center.x - 60.f, center.y - 32.f});
	applyImpactFrame();
}

void FlashThunder::applyImpactFrame() {
	if (!sprite.getTexture() || impactFrame >= IMPACT_FRAME_COUNT) return;

	const float progress = static_cast<float>(impactFrame)
		/ static_cast<float>(IMPACT_FRAME_COUNT - 1);
	const float baseScale =
		STRIKE_RENDER_WIDTH / THUNDER_STRIKE_FRAME.width;
	const float pulseScale = baseScale * (1.f + progress * 0.45f);
	sprite.setTextureRect(THUNDER_STRIKE_FRAME);
	sprite.setOrigin(THUNDER_STRIKE_FRAME.width * 0.5f,
	                 THUNDER_STRIKE_FRAME.height * 0.5f);
	sprite.setScale(travelsRight ? pulseScale : -pulseScale,
	                pulseScale);
	const sf::Uint8 alpha = static_cast<sf::Uint8>(
		255.f - progress * 160.f);
	sprite.setColor(sf::Color(255, 255, 255, alpha));
	sprite.setPosition(position.x + shape.getSize().x * 0.5f,
	                   position.y + shape.getSize().y * 0.5f);
}
