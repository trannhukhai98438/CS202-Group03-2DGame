#include "Entities/Character/Enemy/ThorKing.h"
#include "Entities/Character/Enemy/ThorKingState.h"
#include "Entities/Character/Enemy/Potion.h"
#include <iostream>
#include <cmath>

class BossFire : public Projectile {
private:
    sf::Texture tex;
    sf::Sprite fireSprite;

    void updateSpriteTransform() {
        if (tex.getSize().x == 0 || tex.getSize().y == 0) return;

        const float scaleX = 128.f / static_cast<float>(tex.getSize().x);
        const float scaleY = 128.f / static_cast<float>(tex.getSize().y);
        fireSprite.setScale(velocity.x < 0.f ? -scaleX : scaleX, scaleY);
        fireSprite.setPosition(position.x + shape.getSize().x * 0.5f,
                               position.y + shape.getSize().y * 0.5f);
    }

public:
    BossFire(float startX, float startY, float velX, float velY)
        : Projectile(startX, startY, velX, velY, ProjectileFaction::Enemy, 1) {
        shape.setSize(sf::Vector2f(28.f, 20.f));
        if (tex.loadFromFile("assets/textures/boss_fire.png")) {
            fireSprite.setTexture(tex);
        }
        float baseScale = 0.40f;
        fireSprite.setOrigin(tex.getSize().x / 2.0f, tex.getSize().y / 2.0f);

        float angle = std::atan2(velY, std::abs(velX)) * 180.f / 3.14159265f;
        if (velX < 0) {
            fireSprite.setScale(-baseScale, baseScale);
            fireSprite.setRotation(-angle);
        } else {
            fireSprite.setScale(baseScale, baseScale);
            fireSprite.setRotation(angle);
        }
        setPosition({startX, startY});
    }
    void update(float) override {}

    void render(sf::RenderWindow& window) override {
        if (!isAlive) return;
        if (fireSprite.getTexture() != nullptr) {
            window.draw(fireSprite);
        } else {
            window.draw(shape);
        }
    }

    void setPosition(const sf::Vector2f& pos) override {
        Projectile::setPosition(pos);
        fireSprite.setPosition(position.x + shape.getSize().x * 0.5f,
                               position.y + shape.getSize().y * 0.5f);
    }

    void onSolidCollision(SideType, const sf::FloatRect&) override {
        die();
    }
};

class BossMeteor : public Projectile {
private:
    sf::Texture tex;
    sf::Sprite meteorSprite;
    bool m_isExploding{false};
    float m_animTimer{0.f};
    float m_floorY{0.f};
    bool m_useGravity{true};
public:
    BossMeteor(float startX, float startY, float velX, float velY, float floorY, bool useGravity = true)
        : Projectile(startX, startY, velX, velY, ProjectileFaction::Enemy, 1),
          m_floorY(floorY), m_useGravity(useGravity) {
        shape.setSize(sf::Vector2f(64.f, 64.f));
        shape.setPosition(startX, startY);
        if (tex.loadFromFile("assets/textures/meteor.png")) {
            meteorSprite.setTexture(tex);
        }
        meteorSprite.setScale(1.0f, 1.0f); // Large scale for meteor
        setPosition({startX, startY});
    }

    bool usesWorldPhysics() const override {
        return !m_isExploding;
    }

    void update(float deltaTime) override {
        m_animTimer += deltaTime;
        if (!m_isExploding) {
            if (m_useGravity) {
                // Apply gravity to vertical velocity
                velocity.y += 800.f * deltaTime;
            }

            if (!m_isExploding) {
                // Flying animation (Row 0, 6 frames - horizontal fireballs)
                int frame = static_cast<int>(m_animTimer * 12.0f) % 6;
                static const sf::IntRect flyRects[6] = {
                    sf::IntRect(8, 8, 151, 161),
                    sf::IntRect(176, 8, 158, 161),
                    sf::IntRect(347, 8, 158, 161),
                    sf::IntRect(517, 8, 158, 161),
                    sf::IntRect(687, 8, 158, 161),
                    sf::IntRect(858, 8, 158, 161)
                };
                meteorSprite.setTextureRect(flyRects[frame]);
                meteorSprite.setOrigin(79.f, 80.f);

                // Dynamically rotate sprite to follow the physical velocity angle
                float angle = std::atan2(velocity.y, velocity.x) * 180.f / 3.14159265f;
                meteorSprite.setRotation(angle);
            }
        }

        if (m_isExploding) {
            // Exploding animation (Row 2, 6 frames)
            meteorSprite.setRotation(0.f); // Keep explosion upright on the ground
            int frame = static_cast<int>(m_animTimer * 13.0f);
            if (frame >= 6) {
                die();
                return;
            }
            static const sf::IntRect expRects[6] = {
                sf::IntRect(15, 384, 148, 171),
                sf::IntRect(181, 384, 149, 171),
                sf::IntRect(343, 384, 168, 171),
                sf::IntRect(525, 384, 145, 171),
                sf::IntRect(695, 384, 145, 171),
                sf::IntRect(857, 384, 159, 171)
            };
            static const float expOriginY[6] = {
                140.f, // Frame 0: meteor impact on ground
                110.f, // Frame 1: rock center placed firmly on ground (not floating in air)
                110.f, // Frame 2: rock blast placed firmly on ground
                145.f, // Frame 3: smoke mushroom base on ground
                155.f, // Frame 4: tall mushroom base on ground
                155.f  // Frame 5: debris mound on ground
            };
            meteorSprite.setTextureRect(expRects[frame]);
            meteorSprite.setOrigin(expRects[frame].width / 2.f, expOriginY[frame]);

            // Explode right at ground level flush with the surface (no floating)
            meteorSprite.setPosition(position.x + 32.f, m_floorY);
        }
    }

    void render(sf::RenderWindow& window) override {
        if (tex.getSize().x > 0) {
            window.draw(meteorSprite);
        } else {
            window.draw(shape);
        }
    }

    void setPosition(const sf::Vector2f& pos) override {
        Projectile::setPosition(pos);
        if (!m_isExploding) {
            meteorSprite.setPosition(position.x + shape.getSize().x * 0.5f,
                                     position.y + shape.getSize().y * 0.5f);
        }
    }

    void onSolidCollision(SideType side, const sf::FloatRect& solidBounds) override {
        // Linear meteors disappear on solid contact just like fire.
        if (!m_useGravity) {
            die();
            return;
        }

        // A top-side collision means the falling meteor reached solid ground.
        if (!m_isExploding && side == SideType::Top) {
            m_isExploding = true;
            m_animTimer = 0.f;
            velocity = {0.f, 0.f};
            m_floorY = solidBounds.top;
            position.y = m_floorY - shape.getSize().y;
            shape.setSize(sf::Vector2f(64.f, 64.f));
            shape.setPosition(position.x, position.y);
        }
    }
};

ThorKing::ThorKing(float startX, float startY, std::function<void(std::unique_ptr<Projectile>)> spawnCallback, float patrolRange)
    : Enemy(startX, startY, 60.f, patrolRange), m_bossHp(3), m_rollSpeed(500.f), m_spawnCallback(spawnCallback) {
    shape.setSize(sf::Vector2f(72.f, 116.f));
    health = 3;
    speed = 60.f;

    // Load texture based on prompt
    loadSpriteTexture("assets/textures/boss.png", 1, 128.f, 1);

    animatorComp.initAnimations(*this);
    changeState(std::make_unique<TKPatrolState>());
}

void ThorKing::update(float deltaTime) {
    if (!isAlive) return;
    if (currentState) {
        currentState->update(*this, deltaTime);
    }
}

float ThorKing::getSpeed() const {
    int phase = getPhase();
    float baseWalkSpeed = (phase == 1) ? 70.f : ((phase == 2) ? 110.f : 150.f);
    if (getStateName() == "TKRolling") return getRollSpeed();
    if (getStateName() == "TKPatrol" || getStateName() == "TKFire") return baseWalkSpeed;
    return 0.f;
}

float ThorKing::getRollSpeed() const {
    int phase = getPhase();
    return (phase == 1) ? 500.f : ((phase == 2) ? 620.f : 750.f);
}

float ThorKing::getStunDuration() const {
    int phase = getPhase();
    return (phase == 1) ? 3.0f : ((phase == 2) ? 2.2f : 1.6f);
}

int ThorKing::getDamageOnTouch() const {
    if (getStateName() == "TKStunned") return 0;
    return 2;
}

void ThorKing::onStomped(Character* attacker) {
    physics.onStomped(*this, attacker);
}

void ThorKing::onSideCollision(Character* attacker) {
    physics.onSideCollision(*this, attacker);
}

void ThorKing::takeDamage(int damage) {
    physics.takeDamage(*this, damage);
}

void ThorKing::notifyWallHit() {
    physics.notifyWallHit(*this);
}

void ThorKing::spawnFireProjectile() {
    if (m_spawnCallback) {
        float dirX = (currentDir == MoveDirection::Right) ? 1.0f : -1.0f;
        float sX = position.x + shape.getSize().x / 2.f + dirX * 45.f;
        float sY = position.y + 35.f;

        int phase = getPhase();
        if (phase == 1) {
            m_spawnCallback(std::make_unique<BossFire>(sX, sY, dirX * 400.f, 0.f));
        } else if (phase == 2) {
            m_spawnCallback(std::make_unique<BossFire>(sX, sY, dirX * 480.f, 0.f));
        } else {
            // Phase 3: Alternates between horizontal straight meteor and sky-drop meteor
            int seq = getShotSeq();
            float floorY = position.y + shape.getSize().y;

            if (seq % 2 == 1) {
                // Skill 1: 1 Single Straight Horizontal Meteor (disappears on impact like fire)
                m_spawnCallback(std::make_unique<BossMeteor>(sX, sY, dirX * 500.f, 0.f, floorY, false));
            } else {
                // Skill 2: 1 Single Sky-Drop Meteor (launches high and explodes upon hitting the ground)
                float mouthX = position.x + shape.getSize().x / 2.f + dirX * 20.f;
                float mouthY = position.y - 35.f;
                m_spawnCallback(std::make_unique<BossMeteor>(mouthX, mouthY, dirX * 220.f, -1250.f, floorY, true));
            }
        }
    }
}

void ThorKing::checkObstacles() {
    physics.checkObstacles(*this);
}

void ThorKing::move(float deltaTime) {
    physics.move(*this, deltaTime);
}

void ThorKing::startSkyLaunch() {
    if (m_isSkyLaunching) return;
    m_isSkyLaunching = true;
    m_groundY = position.y;          // remember where the floor is
    m_skyLaunchTimer = 0.f;          // timer starts at liftoff
}

// Sky Launch phases (driven by m_skyLaunchTimer):
//  0.00 – 0.40s : Rise fast upward (~320 px/s)
//  0.40 – 1.50s : Hover at peak (150 px above ground)
//  1.50 – 2.00s : Drop back down fast
//  >= 2.00s     : Land, end launch
void ThorKing::updateSkyLaunch(float dt) {
    m_skyLaunchTimer += dt;
    const float RISE_END   = 0.40f;
    const float HOVER_END  = 1.50f;
    const float LAND_END   = 2.00f;
    const float PEAK_OFFSET = 140.f; // how high above ground

    if (m_skyLaunchTimer < RISE_END) {
        // Rising phase
        float t = m_skyLaunchTimer / RISE_END;
        position.y = m_groundY - PEAK_OFFSET * t;
    } else if (m_skyLaunchTimer < HOVER_END) {
        // Hovering at peak
        position.y = m_groundY - PEAK_OFFSET;
    } else if (m_skyLaunchTimer < LAND_END) {
        // Falling back
        float t = (m_skyLaunchTimer - HOVER_END) / (LAND_END - HOVER_END);
        position.y = (m_groundY - PEAK_OFFSET) + PEAK_OFFSET * t;
    } else {
        // Landed
        position.y = m_groundY;
        m_isSkyLaunching = false;
        m_skyLaunchTimer = 0.f;
    }
    shape.setPosition(position.x, position.y);
}

void ThorKing::applyAnimation() {
    animatorComp.applyAnimation(*this);
}

void ThorKing::render(sf::RenderWindow& window) {
    if (!isAlive) return;
    if (sprite.getTexture() != nullptr) {
        window.draw(sprite);
    } else {
        window.draw(shape);
    }
}
