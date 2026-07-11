#include "Mario.h"

Mario::Mario(float x, float y): Character(x,y){
    currentState = MarioState::Small;
    isInvulnerable = false;
    invulnerableTimer = 0.0f;

    // set up hard status
    moveSpeed=300.0f;
    jumpVelocity=-600.0f;


    //set up sprite and texture
}

void Mario::update(float deltatime){
    // Update invulnerability timer
    if (isInvulnerable) {
        invulnerableTimer -= deltatime;
        if (invulnerableTimer <= 0.0f) {
            isInvulnerable = false;
        }
    }

    // Left movement
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        velocity.x = -moveSpeed;
        facingRight = false;
    // Right movement
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        velocity.x = moveSpeed;
        facingRight = true;
    } else {
        velocity.x = 0.0f; // Stop when not pressing
    }

    // Jump
    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) && isGrounded) {
        velocity.y = jumpVelocity;
        isGrounded = false;
    }

    // Apply gravity
    // velocity.y += gravity * deltatime;

    // Update position
    position += velocity * deltatime;

    // Set position sprite and hitbox
    sprite.setPosition(position);
    hitbox.setPosition(position);
}

void Mario::render(sf::RenderWindow& window){
    window.draw(sprite);
}

void Mario::die(){
    if (isInvulnerable) return;
    
    if (currentState == MarioState::Super || currentState == MarioState::Fire) {
        currentState = MarioState::Small;
        isInvulnerable = true;
        invulnerableTimer = MAX_INVULNERABLE_TIME;
    } else if (currentState == MarioState::Small) {
        currentState = MarioState::Dead;
        Character::die(); // sets isActive = false
    }
}