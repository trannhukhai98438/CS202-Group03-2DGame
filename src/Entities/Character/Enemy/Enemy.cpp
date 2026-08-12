#include "Entities/Character/Enemy/Enemy.h"

Enemy::Enemy(float startX, float startY, float moveSpeed, float patrolRange)
    : Character(startX, startY),
      speed(moveSpeed),
      health(1),
      currentDir(MoveDirection::Left),
      startX(startX),
      patrolLeftBound(startX - patrolRange),
      patrolRightBound(startX + patrolRange),
      currentState(std::make_unique<PatrolState>()) {
}

void Enemy::changeState(std::unique_ptr<EnemyState> newState) {
    if (currentState) {
        currentState->onExit(*this);
    }
    currentState = std::move(newState);
    if (currentState) {
        currentState->onEnter(*this);
    }
}

void Enemy::render(sf::RenderWindow& window) {
    if (!isAlive) return;
    if (sprite.getTexture() != nullptr) {
        window.draw(sprite);
    } else {
        window.draw(shape);
    }
}

void Enemy::setPatrolBounds(float minX, float maxX) {
    patrolLeftBound = minX;
    patrolRightBound = maxX;
}

void Enemy::setPosition(const sf::Vector2f& pos) {
    Character::setPosition(pos);
    sprite.setPosition(position.x + shape.getSize().x / 2.0f, position.y + shape.getSize().y + m_spriteOffsetY);
}

void Enemy::checkObstacles() {
    if (position.x <= patrolLeftBound) {
        setDirection(MoveDirection::Right);
    } else if (position.x >= patrolRightBound) {
        setDirection(MoveDirection::Left);
    }
}

void Enemy::move(float deltaTime) {
    float moveAmount = static_cast<float>(currentDir) * speed * deltaTime;
    setPosition(sf::Vector2f(position.x + moveAmount, position.y));
}

void Enemy::applyAnimation() {
}

int Enemy::interactWith(Character* other) {
    if (!other || !other->getIsAlive()) return 0;
    sf::FloatRect bounds = getBounds();
    sf::FloatRect otherBounds = other->getBounds();

    if (otherBounds.top + otherBounds.height <= bounds.top + 10.0f) {
        onStomped(other);
    } else {
        onSideCollision(other);
    }
    return 0;
}

void Enemy::onStomped(Character* attacker) {
    (void)attacker;
    die();
}

void Enemy::onSideCollision(Character* attacker) {
    if (!attacker) return;
    attacker->takeDamage(getDamageOnTouch());
    flipDirection(); // Turn around when bumping into hero
}

sf::Vector2i Enemy::loadSpriteTexture(const std::string& texturePath, int numCols, float targetHeight, int numRows) {
    sf::Image img;
    if (img.loadFromFile(texturePath)) {
        for (unsigned int y = 0; y < img.getSize().y; ++y) {
            for (unsigned int x = 0; x < img.getSize().x; ++x) {
                sf::Color c = img.getPixel(x, y);
                if (c.r > 190 && c.g > 190 && c.b > 190) {
                    img.setPixel(x, y, sf::Color::Transparent);
                }
            }
        }
        texture.loadFromImage(img);
        sprite.setTexture(texture);
        
        sf::Vector2u texSize = texture.getSize();
        int frameWidth = static_cast<int>(texSize.x / numCols);
        int frameHeight = static_cast<int>(texSize.y / numRows);
        
        float scale = targetHeight / static_cast<float>(frameHeight);
        sprite.setScale(scale, scale);

        int bottomPadding = 0;
        for (int y = frameHeight - 1; y >= 0; --y) {
            bool found = false;
            for (unsigned int x = 0; x < img.getSize().x; ++x) {
                if (img.getPixel(x, y).a > 0) {
                    found = true;
                    break;
                }
            }
            if (found) {
                bottomPadding = frameHeight - 1 - y;
                break;
            }
        }
        setSpriteOffsetY(static_cast<float>(bottomPadding) * scale);
        
        return sf::Vector2i(frameWidth, frameHeight);
    }
    return sf::Vector2i(0, 0);
}
