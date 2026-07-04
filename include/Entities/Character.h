#pragma once
#include <SFML/Graphics.hpp>

class Character{
    protected:
        sf::Vector2f position;
        sf::Vector2f velocity;
        sf::RectangleShape hitbox;
        sf::Sprite sprite;
        sf::Texture texture;
        int hp; //temp
        bool facingRight;
        // virtual void specialAbility()=0;
    public:
        Character(float x, float y);
        virtual ~Character()=default;

        virtual void update(float deltatime)=0;
        virtual void render(sf::RenderWindow & window)=0;
        virtual void takedamage(int x); // if the character have hp
        sf::FloatRect getBounds();
        bool isDead() const;
};