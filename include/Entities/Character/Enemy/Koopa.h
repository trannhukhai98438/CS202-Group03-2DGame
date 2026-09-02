#ifndef KOOPA_H
#define KOOPA_H

#include "Enemy.h"
#include "KoopaPhysics.h"
#include "KoopaAnimator.h"

class Koopa : public Enemy {
private:
    float shellSpeed;
    float kickCooldown = 0.0f;
    float shellTimer = 0.0f;
    KoopaPhysics physics;
    KoopaAnimator animatorComp;

public:
    float getKickCooldown() const { return kickCooldown; }
    void setKickCooldown(float val) { kickCooldown = val; }
    float getShellTimer() const { return shellTimer; }
    void setShellTimer(float val) { shellTimer = val; }
    void wakeUp();
    Koopa(float startX, float startY, float patrolRange = 120.0f);

    void update(float deltaTime) override;
    float getSpeed() const override;
    int getDamageOnTouch() const override;
    int getScoreValue() const override { return 200; }
    std::string getCharacterType() const override { return "Koopa"; }
    float getShellSpeed() const { return shellSpeed; }

    void render(sf::RenderWindow& window) override;
    void takeDamage(int damage) override;
    void becomeStaticShell();
    void onStomped(Character* attacker) override;
    void onSideCollision(Character* attacker) override;

    bool getIsShell() const;
    bool getIsShellSpinning() const;
    void kickShell(MoveDirection dir);

protected:
    void checkObstacles() override;
    void move(float deltaTime) override;
    void applyAnimation() override;
};

#endif
