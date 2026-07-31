#pragma once
#include "Character.h"
#include "HeroForm.h"
#include "HeroState.h"
#include <memory>


class Hero: public Character{
protected:
    std::unique_ptr<HeroForm> form; // Small/Giant/Fire
    std::unique_ptr<HeroState> state; // Idle/Run/Jump/Fireball (only FireMario)
    std::string baseTexturePath;
    std::string overrideAnim;
    float overrideTimer;
    int coin;
public:
    Hero(float x, float y);
    virtual ~Hero()=default;
    void loadTexture(const std::string& path);
    std::string getBaseTexturePath() const;
    void update(float deltatime) override;
    void render(sf::RenderWindow& window) override;

    void takedamage() override;
    void die() override;
    void setForm(std::unique_ptr<HeroForm> newForm);
    void setState(std::unique_ptr<HeroState> newState);
    std::string getStateName() const;
    virtual void specialAbility();
    void playOverrideAnimation(const std::string& animName, float duration);
};