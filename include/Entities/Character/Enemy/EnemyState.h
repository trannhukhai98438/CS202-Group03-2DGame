#ifndef ENEMY_STATE_H
#define ENEMY_STATE_H

#include <string>
#include <memory>

class Enemy;

class EnemyState {
public:
    virtual ~EnemyState() = default;
    
    virtual void onEnter(Enemy& enemy) { (void)enemy; }
    virtual void update(Enemy& enemy, float deltaTime) = 0;
    virtual void onExit(Enemy& enemy) { (void)enemy; }
    virtual std::string getName() const = 0;
};

class PatrolState : public EnemyState {
public:
    void update(Enemy& enemy, float deltaTime) override;
    std::string getName() const override { return "Patrol"; }
};

class SquishedState : public EnemyState {
private:
    float timer;
    float fadeDuration;
public:
    SquishedState(float duration = 0.6f, float fadeTime = 0.3f, float initialTimer = 0.0f);
    void onEnter(Enemy& enemy) override;
    void update(Enemy& enemy, float deltaTime) override;
    std::string getName() const override { return "Squished"; }

    float getTimer() const { return timer; }
    void setTimer(float t) { timer = t; }
};

class ShellState : public EnemyState {
public:
    void onEnter(Enemy& enemy) override;
    void update(Enemy& enemy, float deltaTime) override;
    std::string getName() const override { return "Shell"; }
};

class SpinningShellState : public EnemyState {
public:
    void onEnter(Enemy& enemy) override;
    void update(Enemy& enemy, float deltaTime) override;
    std::string getName() const override { return "SpinningShell"; }
};

class FlippingDeathState : public EnemyState {
private:
    float velocityY;
public:
    FlippingDeathState(float initialVelY = -150.0f);
    void onEnter(Enemy& enemy) override;
    void update(Enemy& enemy, float deltaTime) override;
    std::string getName() const override { return "FlippingDeath"; }
};

#endif
