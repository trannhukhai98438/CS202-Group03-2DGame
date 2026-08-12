#pragma once
#include <SFML/Graphics.hpp>
#include "Utilities/Animator.h"
#include "Entities/Character/Hero/HeroForm/HeroForm.h"
#include "Entities/Character/Hero/HeroState/HeroState.h"
#include <memory>

#include "Entities/Character/Character.h"
class Item; // forward declaration — full definition in Hero.cpp via #include "Item.h"

class Hero : public Character {
protected:
    std::unique_ptr<HeroForm> form;   // Small / Giant / Fire
    std::unique_ptr<HeroState> state; // Idle / Run / Jump / Sit / Slide / Dead / Fly / Grow / Shrink
    std::string baseTexturePath;
    std::string overrideAnim;
    float overrideTimer;
    float invincibleTimer;
    bool isStarman;
    int coin;
    int hp;
public:
    Hero(float x, float y);
    virtual ~Hero()=default;
    void loadTexture(const std::string& path);
    std::string getBaseTexturePath() const;
    void update(float deltatime) override;
    void render(sf::RenderWindow& window) override;

    void takedamage();
    void die() override;
    void setForm(std::unique_ptr<HeroForm> newForm);
    void setState(std::unique_ptr<HeroState> newState);
    std::string getStateName() const;
    std::string getFormName() const;
    virtual void specialAbility();
    void playOverrideAnimation(const std::string& animName, float duration);
    void setInvincible(float duration, bool starman = false);
    int interactWith(Character* other) override;
    void collectItem(Item* item);

    void collectCoin();
    int getCoin() const;
    bool isDead() const;
    int getHp();
    void setSize(float x, float y);
    void setPosition(float x, float y);
};