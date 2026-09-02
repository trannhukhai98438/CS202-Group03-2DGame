#ifndef THOR_KING_H
#define THOR_KING_H

#include "Enemy.h"
#include "Projectile.h"
#include "ThorKingPhysics.h"
#include "ThorKingAnimator.h"
#include "ThorKingState.h"
#include <functional>
#include <memory>

enum class BossSoundEvent {
    Attack,
    Defeated
};

class ThorKing : public Enemy {
private:
    int   m_bossHp;          // 3 HP total
    float m_rollSpeed;       // speed when rolling shell
    int   m_fireCount = 0;   // fire attack count to trigger roll
    int   m_wallBounceCount = 0; // double bounce tracking for phase 3
    // Phase 3 Sky Launch mechanic
    bool  m_isSkyLaunching{false};
    float m_skyLaunchTimer{0.f};
    float m_groundY{0.f};    // remembered ground Y to return to
    int   m_shotSeq = 0;     // tracks phase 3 alternating skills
    ThorKingPhysics  physics;
    ThorKingAnimator animatorComp;
    std::function<void(std::unique_ptr<Projectile>)> m_spawnCallback;
    std::function<void(BossSoundEvent)> m_soundCallback;
    bool m_pendingDieSound = false;
    float m_dieSoundTimer = 0.0f;
    const float DIE_SOUND_DELAY = 0.15f;

public:
    ThorKing(float startX, float startY,
             std::function<void(std::unique_ptr<Projectile>)> spawnCallback,
             float patrolRange = 200.0f);
    ~ThorKing() override = default;

    void setSoundCallback(std::function<void(BossSoundEvent)> soundCallback) { m_soundCallback = std::move(soundCallback); }

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
    int   getMaxHp() const   { return 3; }
    int   getPhase() const   { return (m_bossHp >= 3) ? 1 : ((m_bossHp == 2) ? 2 : 3); }
    void  setBossHp(int hp)  { m_bossHp = hp; health = hp; }
    float getRollSpeed() const;
    int   getFireBurstCount() const { return getPhase(); }
    float getStunDuration() const;
    void  notifyWallHit();    // called by PlayingState when shell hits a wall
    void  setIsAlive(bool alive) { isAlive = alive; }
    bool  getIsAlive() const { return isAlive; }

    int   getFireCount() const { return m_fireCount; }
    void  incrementFireCount() { m_fireCount++; }
    void  resetFireCount() { m_fireCount = 0; }

    int   getWallBounceCount() const { return m_wallBounceCount; }
    void  incrementWallBounceCount() { m_wallBounceCount++; }
    void  resetWallBounceCount() { m_wallBounceCount = 0; }

    int   getShotSeq() const { return m_shotSeq; }
    void  incrementShotSeq() { m_shotSeq++; }
    void  resetShotSeq() { m_shotSeq = 0; }

    // Phase 3 Sky Launch accessors
    bool  isSkyLaunching() const { return m_isSkyLaunching; }
    void  startSkyLaunch();    // kick boss into the air for meteor attack
    void  updateSkyLaunch(float dt); // drive the rise/hover/fall cycle
    float getGroundY() const { return m_groundY; }
    void  setGroundY(float y) { m_groundY = y; }

    void spawnFireProjectile();

    void restoreBossState(int hp, int fireCount, int wallBounce, int shotSeq, bool isSkyLaunch, float groundY);

public:
    void checkObstacles() override;
    void move(float deltaTime) override;
    void applyAnimation() override;
};

#endif
