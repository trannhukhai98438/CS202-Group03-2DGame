#pragma once

#include "Entities/Character/Enemy/Projectile.h"

#include <array>
#include <unordered_set>

class FlashThunder : public Projectile {
private:
	enum class Phase { Flying, Impact };

	sf::Sprite sprite;
	Phase phase{Phase::Flying};
	float lifetime{1.5f};
	float frameTimer{0.f};
	std::size_t impactFrame{0};
	bool damageWindowOpen{false};
	std::unordered_set<const Character*> hitTargets;
	const std::array<sf::IntRect, 4> impactFrames{{
		{1190, 638, 70, 68}, {1288, 628, 89, 88},
		{1404, 614, 93, 108}, {1521, 614, 105, 107}
	}};

	void enterImpact();
	void applyImpactFrame();

public:
	FlashThunder(float startX, float startY, float velocityX);
	void update(float deltaTime) override;
	void render(sf::RenderWindow& window) override;
	float getGravityAcceleration() const override { return 0.f; }
	bool usesWorldPhysics() const override { return phase == Phase::Flying; }
	void onSolidCollision(SideType side,
	                      const sf::FloatRect& solidBounds) override;
	bool onHitTarget(Character& target) override;
	void onTargetResolutionComplete() override;
	void setPosition(const sf::Vector2f& pos) override;
};
