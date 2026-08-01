#include "Entities/Enemy.h"

Enemy::Enemy(float startX, float startY, float moveSpeed, float patrolRange)
    : BaseEntity(startX, startY),
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

void Enemy::checkObstacles() {
    if (position.x <= patrolLeftBound) {
        setDirection(MoveDirection::Right);
    } else if (position.x >= patrolRightBound) {
        setDirection(MoveDirection::Left);
    }
}

void Enemy::move(float deltaTime) {
    float moveAmount = static_cast<float>(currentDir) * speed * deltaTime;
    position.x += moveAmount;
    shape.setPosition(position);
    sprite.setPosition(position.x + shape.getSize().x / 2.0f, position.y + shape.getSize().y + m_spriteOffsetY);
}

void Enemy::applyAnimation() {
}

void Enemy::interactWith(BaseEntity* other) {
    if (!other || !other->getIsAlive()) return;
    sf::FloatRect bounds = getBounds();
    sf::FloatRect otherBounds = other->getBounds();

    if (otherBounds.top + otherBounds.height <= bounds.top + 10.0f) {
        onStomped(other);
    } else {
        onSideCollision(other);
    }
}

void Enemy::onStomped(BaseEntity* attacker) {
    (void)attacker;
    die();
}

void Enemy::onSideCollision(BaseEntity* attacker) {
    if (!attacker) return;
    attacker->takeDamage(getDamageOnTouch());
}

sf::Vector2i Enemy::loadSpriteTexture(const std::string& texturePath, int numFrames, float targetHeight) {
    sf::Image img;
    if (img.loadFromFile(texturePath)) {
        for (unsigned int y = 0; y < img.getSize().y; ++y) {
            for (unsigned int x = 0; x < img.getSize().x; ++x) {
                sf::Color c = img.getPixel(x, y);
                if (c.r > 230 && c.g > 230 && c.b > 230) {
                    img.setPixel(x, y, sf::Color::Transparent);
                }
            }
        }
        texture.loadFromImage(img);
        sprite.setTexture(texture);
        
        sf::Vector2u texSize = texture.getSize();
        int frameWidth = static_cast<int>(texSize.x / numFrames);
        int frameHeight = static_cast<int>(texSize.y);
        
        float scale = targetHeight / static_cast<float>(frameWidth);
        sprite.setScale(scale, scale);

        int bottomPadding = 0;
        for (int y = img.getSize().y - 1; y >= 0; --y) {
            bool found = false;
            for (unsigned int x = 0; x < img.getSize().x; ++x) {
                if (img.getPixel(x, y).a > 0) {
                    found = true;
                    break;
                }
            }
            if (found) {
                bottomPadding = img.getSize().y - 1 - y;
                break;
            }
        }
        setSpriteOffsetY(static_cast<float>(bottomPadding) * scale);
        
        return sf::Vector2i(frameWidth, frameHeight);
    }
    return sf::Vector2i(0, 0);
}
