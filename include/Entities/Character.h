#pragma once
#include <SFML/Graphics.hpp>
#include "../Utilities/Animator.h"

class Character{
    protected:
        sf::Vector2f position;
        sf::Vector2f velocity;
        sf::RectangleShape hitbox;
        sf::Sprite sprite;
        sf::Texture texture;
        Animator animator;
        bool isActive; // Replaces hp for alive/dead state
        bool facingRight;
        bool isGrounded;
        // virtual void specialAbility()=0;
    public:
        Character(float x, float y); 
        // Constructor of Derived classes calculate sprite frames from sprite texture
        // and set up the animator by themselves because they have different sprite frames.
        virtual ~Character()=default;

        virtual void update(float deltatime)=0;
        virtual void render(sf::RenderWindow & window)=0;
        virtual void die(); // Replaces takedamage
        sf::FloatRect getBounds();
        bool isDead() const;
};