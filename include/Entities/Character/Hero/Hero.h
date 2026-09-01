#pragma once
#include <SFML/Graphics.hpp>
#include "Utilities/Animator.h"
#include "Entities/Character/Hero/HeroForm/HeroForm.h"
#include "Entities/Character/Hero/HeroState/HeroState.h"
#include "Entities/Character/Enemy/Projectile.h"
#include <functional>
#include <memory>

#include "Entities/Character/Character.h"

class Item; // forward declaration — full definition in Hero.cpp via #include "Item.h"


using ProjectileSpawnCallback = std::function<void(std::unique_ptr<Projectile>)>;

class Hero : public Character {
protected:
    std::unique_ptr<HeroForm> form;   // Small / Giant / Fire
    std::unique_ptr<HeroState> state; // Idle / Run / Jump / Sit / Slide / Dead / Cheer / Grow / Shrink
    std::string baseTexturePath;
	std::string specialTexturePath;
	float spriteRenderScale{2.f};
	float specialSpriteRenderScale{0.f};
    std::string overrideAnim;
    float overrideTimer;
    float invincibleTimer;
    bool isStarman;
    int coin;
    int hp;
    ProjectileSpawnCallback spawnProjectileCallback;
    virtual std::unique_ptr<Projectile> createSpecialProjectile() const = 0;
public:
    Hero(float x, float y, ProjectileSpawnCallback spawnCallback = nullptr);
    virtual ~Hero()=default;
    virtual void loadTexture(const std::string& path);
    std::string getBaseTexturePath() const;
	std::string getSpecialTexturePath() const;
    void update(float deltatime) override;
    void render(sf::RenderWindow& window) override;

    std::function<void(const std::string&)> playSFXCallback;
    void playSFX(const std::string& name);

    void takeDamage(int damage) override;
    void die() override;
    void setForm(const std::string& newForm);
    void setForm(std::unique_ptr<HeroForm> newForm);
    void setState(std::unique_ptr<HeroState> newState);
    std::string getStateName() const;
    std::string getFormName() const;
    bool specialAbility();
    virtual std::string getHeroType() const = 0;
    virtual float getSpecialCooldown() const = 0;
    void playOverrideAnimation(const std::string& animName, float duration);
    void setInvincible(float duration, bool starman = false);
    int interactWith(Character* other) override;
    void onStomped(Character* attacker) override;
    void onSideCollision(Character* attacker) override;
    void collectItem(Item* item);
    
    float getInvincibleTimer() const;
    bool getIsStarman() const;
    void collectCoin();
    int getCoin() const;
    void setCoin(int newCoin);
    bool isDead() const;
    int getHp() const;
    void setHp(int newHp);
    void setSize(float x, float y);
    void setPosition(float x, float y);
};
