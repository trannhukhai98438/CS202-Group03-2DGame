#ifndef THOR_KING_H
#define THOR_KING_H

#include "Enemy.h"
#include "Projectile.h"
#include "ThorKingPhysics.h"
#include "ThorKingAnimator.h"
#include "ThorKingState.h"
#include <functional>
#include <memory>

class ThorKing : public Enemy {
private:
    int   m_bossHp;          // 3 HP total
    float m_rollSpeed;       // speed when rolling shell
    int   m_fireCount = 0;   // fire attack count to trigger roll
    ThorKingPhysics  physics;
    ThorKingAnimator animatorComp;
    std::function<void(std::unique_ptr<Projectile>)> m_spawnCallback;

public:
    ThorKing(float startX, float startY,
             std::function<void(std::unique_ptr<Projectile>)> spawnCallback,
             float patrolRange = 200.0f);
    ~ThorKing() override = default;

    // Overrides from Enemy / Character
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    float getSpeed() const override;
    int   getDamageOnTouch() const override;
    int   getScoreValue() const override { return 1000; }
    std::string getCharacterType() const override { return "ThorKing"; }

    void onStomped(Character* attacker) override;
    void onSideCollision(Character* attacker) override;
    void takeDamage(int damage) override;

    // Boss-specific accessors
    int   getBossHp() const  { return m_bossHp; }
    void  setBossHp(int hp)  { m_bossHp = hp; health = hp; }
    float getRollSpeed() const { return m_rollSpeed; }
    void  notifyWallHit() override;
    void  setIsAlive(bool alive) { isAlive = alive; }
    bool  getIsAlive() const { return isAlive; }

    int   getFireCount() const { return m_fireCount; }
    void  incrementFireCount() { m_fireCount++; }
    void  resetFireCount() { m_fireCount = 0; }

    void spawnFireProjectile();

public:
    void checkObstacles() override;
    void move(float deltaTime) override;
    void applyAnimation() override;
};

#endif
