#include "Core/PlayingState.h"
#include "Core/Game.h"
#include "Core/PausedState.h"
#include "Core/GameOverState.h"
#include "Core/VictoryState.h"
#include "Entities/Character/Hero/HeroFactory.h"
#include "Entities/Block/BlockFactory.h"
#include "Entities/Item/ItemFactory.h"
#include <algorithm>


PlayingState::PlayingState() : m_physics(), m_hudManager(), m_lastCoinCount(0) {
    m_camera.setSize(1280.f, 720.f);
    m_hudManager.init("assets/fonts/SuperMario256.ttf");
    hero = HeroFactory().createHero(HeroType::Mario, 100.f, 500.f);

    m_dummyFloor.setSize(sf::Vector2f(1280.f, 120.f));
    m_dummyFloor.setFillColor(sf::Color::Green);
    m_dummyFloor.setPosition(0.f, 600.f);

    m_dummyWall.setSize(sf::Vector2f(50.f, 200.f));
    m_dummyWall.setFillColor(sf::Color::Yellow);
    m_dummyWall.setPosition(800.f, 400.f);

    BlockFactory blockFac;
    blocks.push_back(blockFac.createBlock(BlockType::Brick, 250, 475));
    blocks.push_back(blockFac.createBlock(BlockType::Question, 300, 475, ItemType::PowerUpPrototype));
    blocks.push_back(blockFac.createBlock(BlockType::Question, 275, 350, ItemType::PowerUpPrototype));
    blocks.push_back(blockFac.createBlock(BlockType::Question, 350, 475, ItemType::Coin));
    blocks.push_back(blockFac.createBlock(BlockType::Question, 350, 350, ItemType::Star));

    auto spawnCallback = [this](std::unique_ptr<Projectile> p) {
        m_projectiles.push_back(std::move(p));
    };

    m_enemies.push_back(EnemyFactory::createEnemy(EnemyType::Goomba, 300.f, 536.f, 150.f, spawnCallback));
    m_enemies.push_back(EnemyFactory::createEnemy(EnemyType::Koopa, 900.f, 550.f, 200.f, spawnCallback));
    m_enemies.push_back(EnemyFactory::createEnemy(EnemyType::Witch, 700.f, 504.f, 150.f, spawnCallback));
}

PlayingState::~PlayingState() = default;

void PlayingState::processEvents(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab) {
        Game::getInstance().pushState(std::make_unique<PausedState>());
    }

    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C) {
        m_hudManager.addCoin(1);
        m_hudManager.addScore(100);
    }
}

void PlayingState::update(sf::Time dt) {
    float dtSec = dt.asSeconds();
    m_hudManager.updateTimer(dtSec);

    // Update hero, blocks, items
    if (hero) {
        hero->update(dtSec);
        int currentCoins = hero->getCoin();
        if (currentCoins > m_lastCoinCount) {
            int diff = currentCoins - m_lastCoinCount;
            m_hudManager.addCoin(diff);
            m_hudManager.addScore(100 * diff);
            m_lastCoinCount = currentCoins;
        }
    }
    for (size_t i = 0; i < blocks.size(); ++i) blocks[i]->update(dtSec);
    for (size_t i = 0; i < items.size(); ++i) items[i]->update(dtSec);

    if (hero && !hero->isDead()) {
        sf::Vector2f oldpos = hero->getPosition();
        sf::Vector2f vel = hero->getVelocity();

        hero->setPosition(oldpos.x + vel.x * dtSec, oldpos.y);
        m_physics.resolveCollisionX(hero->getHitbox(), m_dummyFloor, vel.x);
        m_physics.resolveCollisionX(hero->getHitbox(), m_dummyWall, vel.x);
        for (size_t i = 0; i < blocks.size(); ++i) {
            if (blocks[i]->getIsActive()) {
                m_physics.resolveCollisionX(hero->getHitbox(), blocks[i]->getHitbox(), vel.x);
            }
        }

        hero->setPosition(hero->getHitbox().getPosition().x, oldpos.y + vel.y * dtSec);
        bool grounded = false;
        if (m_physics.checkCollision(hero->getHitbox(), m_dummyFloor) == SideType::Top) grounded = true;
        m_physics.resolveCollisionY(hero->getHitbox(), m_dummyFloor, vel.y);
        if (m_physics.checkCollision(hero->getHitbox(), m_dummyWall) == SideType::Top) grounded = true;
        m_physics.resolveCollisionY(hero->getHitbox(), m_dummyWall, vel.y);
        for (size_t i = 0; i < blocks.size(); ++i) {
            if (blocks[i]->getIsActive()) {
                if (m_physics.checkCollision(hero->getHitbox(), blocks[i]->getHitbox()) == SideType::Top) grounded = true;

                float oldVelY = vel.y;
                m_physics.resolveCollisionY(hero->getHitbox(), blocks[i]->getHitbox(), vel.y);

                if (vel.y == 0.f && oldVelY < 0.f) {
                    if (auto spawnedItem = blocks[i]->hit(hero.get())) {
                        items.push_back(std::move(spawnedItem));
                    }
                }
            }
        }

        hero->setGrounded(grounded);
        sf::Vector2f pos = hero->getHitbox().getPosition();
        hero->setPosition(pos.x, pos.y);
        hero->setVelocity(vel.x, vel.y);
    }

    // Update items
    for (auto& item : items) {
        sf::Vector2f oldpos = item->getPosition();
        sf::Vector2f vel = item->getVelocity();
        if (item->isColliable()) {
            float oldVelX = vel.x;
            item->setPosition(oldpos.x + vel.x * dtSec, oldpos.y);
            m_physics.resolveCollisionX(item->getHitbox(), m_dummyFloor, vel.x);
            m_physics.resolveCollisionX(item->getHitbox(), m_dummyWall, vel.x);
            for (size_t i = 0; i < blocks.size(); ++i) {
                if (blocks[i]->getIsActive()) {
                    m_physics.resolveCollisionX(item->getHitbox(), blocks[i]->getHitbox(), vel.x);
                }
            }

            if (oldVelX != 0.f && vel.x == 0.f) {
                item->getCollision();
                vel = item->getVelocity();
            }

            item->setPosition(item->getHitbox().getPosition().x, oldpos.y + vel.y * dtSec);
            bool grounded = false;
            if (m_physics.checkCollision(item->getHitbox(), m_dummyFloor) == SideType::Top) grounded = true;
            m_physics.resolveCollisionY(item->getHitbox(), m_dummyFloor, vel.y);
            if (m_physics.checkCollision(item->getHitbox(), m_dummyWall) == SideType::Top) grounded = true;
            m_physics.resolveCollisionY(item->getHitbox(), m_dummyWall, vel.y);
            for (size_t i = 0; i < blocks.size(); ++i) {
                if (blocks[i]->getIsActive()) {
                    if (m_physics.checkCollision(item->getHitbox(), blocks[i]->getHitbox()) == SideType::Top) grounded = true;
                    m_physics.resolveCollisionY(item->getHitbox(), blocks[i]->getHitbox(), vel.y);
                }
            }

            item->setGrounded(grounded);
            sf::Vector2f pos = item->getHitbox().getPosition();
            item->setPosition(pos.x, pos.y);
            item->setVelocity(vel.x, vel.y);
        } else {
            item->setPosition(oldpos.x + vel.x * dtSec, oldpos.y + vel.y * dtSec);
        }
    }

    // Hero collecting items
    if (hero && !hero->isDead()) {
        for (auto& item : items) {
            if (item->isColliable() && m_physics.checkCollision(hero->getHitbox(), item->getHitbox()) != SideType::None) {
                hero->collectItem(item.get());
            }
        }
    }

    // Clean up inactive blocks and collected items
    blocks.erase(std::remove_if(blocks.begin(), blocks.end(),
        [](const std::unique_ptr<Block>& block) { return !block->getIsActive(); }),
        blocks.end());
    items.erase(std::remove_if(items.begin(), items.end(),
        [](const std::unique_ptr<Item>& item) { return item->isCollected(); }),
        items.end());

    // Update enemies
    for (auto it = m_enemies.begin(); it != m_enemies.end();) {
        (*it)->update(dtSec);

        if ((*it)->getIsAlive() && (*it)->getStateName() != "FlippingDeath" && (*it)->getStateName() != "Squished" && hero && !hero->isDead() && (*it)->getBounds().intersects(hero->getHitbox().getGlobalBounds())) {
            sf::FloatRect enemyBounds = (*it)->getBounds();
            sf::FloatRect heroBounds = hero->getHitbox().getGlobalBounds();

            bool isFallingInAir = (!hero->getGrounded() && hero->getVelocity().y >= 0.f);
            float marioBottomY = heroBounds.top + heroBounds.height;
            float enemyTopY = enemyBounds.top;

            // Stomp logic: Mario must be falling from the air and hit the top half of the enemy
            if (isFallingInAir && marioBottomY <= enemyTopY + (enemyBounds.height * 0.6f)) {
                (*it)->onStomped(nullptr);
                hero->setVelocity(hero->getVelocity().x, -300.f); // Bounce Mario up slightly!
                m_hudManager.addScore(200);
            } else {
                (*it)->onSideCollision(nullptr);
                if ((*it)->getDamageOnTouch() > 0) {
                    hero->takedamage();
                }
            }
        }

        if (!(*it)->getIsAlive()) {
            it = m_enemies.erase(it);
        } else {
            ++it;
        }
    }

    // Update projectiles
    for (auto it = m_projectiles.begin(); it != m_projectiles.end();) {
        (*it)->update(dtSec);
        if ((*it)->getIsAlive() && hero && !hero->isDead() && (*it)->getBounds().intersects(hero->getHitbox().getGlobalBounds())) {
            (*it)->die();
            hero->takedamage();
        }
        if (!(*it)->getIsAlive()) {
            it = m_projectiles.erase(it);
        } else {
            ++it;
        }
    }
    if (hero->getPosition().y> 720.f) {
        hero->die();
    }
    float marioX = hero->getPosition().x;
    float halfScreenWidth = 640.f;
    float levelEnd = 5000.f; //Wherever the level ends. This is just a PLACEHOLDER for now.
    float cameraX = std::clamp(marioX, halfScreenWidth, levelEnd - halfScreenWidth);
    m_camera.setCenter(cameraX, 360.f);
	// TEST SCREENS (delete this when we have a proper Mario sprite and level assets)
    // Press 'L' to simulate Mario dying
    if (hero->isDead()) {
        Game::getInstance().changeState(std::make_unique<GameOverState>());
    }

    // Press 'W' to simulate touching the flagpole
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        Game::getInstance().changeState(std::make_unique<VictoryState>());
    }
}

void PlayingState::render(sf::RenderWindow& window) {
    window.draw(m_dummyFloor);
    window.draw(m_dummyWall);
    for (auto& block : blocks) if (block) block->render(window);
    for (auto& item : items) if (item) item->render(window);
    if (hero) hero->render(window);

    for (auto& enemy : m_enemies) {
        if (enemy) enemy->render(window);
    }

    for (auto& projectile : m_projectiles) {
        if (projectile) projectile->render(window);
    }

    window.draw(m_hudManager);


    window.setView(m_camera);
}