#pragma once
#include <SFML/Graphics.hpp>
#include "Utilities/Animator.h"
#include "Entities/Character/Hero/HeroForm/HeroForm.h"
#include "Entities/Character/Hero/HeroState/HeroState.h"
#include <memory>

class Item; // forward declaration — full definition in Hero.cpp via #include "Item.h"

class Hero{
protected:
    std::unique_ptr<HeroForm> form;   // Small / Giant / Fire
    std::unique_ptr<HeroState> state; // Idle / Run / Jump / Sit / Slide / Dead / Fly / Grow / Shrink
    std::string baseTexturePath;
    std::string overrideAnim;
    float overrideTimer;
    float invincibleTimer;
    bool isStarman;
    int coin;
protected:
    sf::Sprite sprite;
    sf::Texture texture;
    Animator animator;
    sf::Vector2f velocity;
    sf::Vector2f position;
    sf::RectangleShape hitbox;
    bool isFacingRight;
    bool isGrounded;    // managed externally by CollisionSystem //TODO
    bool isActive;      // false when character is dead / removed from scene
    int hp;
public:
    Hero(float x, float y);
    virtual ~Hero()=default;
    void loadTexture(const std::string& path);
    std::string getBaseTexturePath() const;
    void update(float deltatime);
    void render(sf::RenderWindow& window);

    void takedamage();
    void die();
    void setForm(std::unique_ptr<HeroForm> newForm);
    void setState(std::unique_ptr<HeroState> newState);
    std::string getStateName() const;
    std::string getFormName() const;
    virtual void specialAbility();
    void playOverrideAnimation(const std::string& animName, float duration);
    void setInvincible(float duration, bool starman = false);
    void collectItem(Item* item);

    void collectCoin();
    int getCoin() const;
    bool isDead();
    sf::FloatRect getBounds();
    sf::RectangleShape& getHitbox();
    int getHp();
    void setSize(float x, float y);
    void setVelocity(float x, float y);
    sf::Vector2f getVelocity();
    void setPosition(float x, float y);
    sf::Vector2f getPosition();
    bool getGrounded();
    void setGrounded(bool grounded);
    void setFacingRight(bool facing);
    bool getFacingRight() const;
};