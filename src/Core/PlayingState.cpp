#include "Core/PlayingState.h"
#include "Core/Game.h"
#include "Core/PausedState.h"
#include "Core/GameOverState.h"
#include "Core/VictoryState.h"
#include "Entities/Character/Hero/HeroFactory.h"
#include "Entities/Block/BlockFactory.h"
#include "Entities/Item/ItemFactory.h"
#include <algorithm>
#include <iostream>
PlayingState::PlayingState() : m_physics(), m_hudManager(), m_lastCoinCount(0) {
    m_hudManager.init("assets/fonts/SuperMario256.ttf");
    std::string jsonPath = "assets/maps/levels/1-1.tmj";
    std::string tilesetPath = "assets/maps/resources/tileset.png";
    if (!m_levelManager.loadLevel(jsonPath, tilesetPath)) {
        std::cerr << "[PlayingState] ERROR: Cannot load level!" << std::endl;
    }
    MapObject spawnPoint;
    if (m_levelManager.getObjectByName("Objects", "SpawnPoint", spawnPoint)) {
        m_hero=HeroFactory().createHero(HeroType::Luigi,spawnPoint.x,spawnPoint.y);
    } else {
        m_hero=HeroFactory().createHero(HeroType::Luigi,100,0);
    }
    m_dummyFloor.setSize(sf::Vector2f(1280.f, 120.f));
    m_dummyFloor.setFillColor(sf::Color::Green);
    m_dummyFloor.setPosition(0.f, 600.f);

    m_dummyWall.setSize(sf::Vector2f(50.f, 200.f));
    m_dummyWall.setFillColor(sf::Color::Yellow);
    m_dummyWall.setPosition(800.f, 400.f);

    auto spawnCallback = [this](std::unique_ptr<Projectile> p) {
        m_projectiles.push_back(std::move(p));
    };

    m_enemies.push_back(EnemyFactory::createEnemy(EnemyType::Goomba, 300.f, 536.f, 150.f, spawnCallback));
    m_enemies.push_back(EnemyFactory::createEnemy(EnemyType::Koopa, 600.f, 550.f, 200.f, spawnCallback));
    m_enemies.push_back(EnemyFactory::createEnemy(EnemyType::Witch, 900.f, 504.f, 150.f, spawnCallback));

    BlockFactory blockFac;
    m_blocks.push_back(blockFac.createBlock(BlockType::Brick, 300,550));
    m_blocks.push_back(blockFac.createBlock(BlockType::Question,400, 550, ItemType::PowerUpPrototype));
    m_blocks.push_back(blockFac.createBlock(BlockType::Question,420, 550, ItemType::PowerUpPrototype));
    m_blocks.push_back(blockFac.createBlock(BlockType::Question,440, 550, ItemType::Coin));
    m_blocks.push_back(blockFac.createBlock(BlockType::Question,460, 550, ItemType::Star));
}

PlayingState::~PlayingState() = default;

void PlayingState::processEvents(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab) {
        Game::getInstance().pushState(std::make_unique<PausedState>());
    }
}

void PlayingState::update(sf::Time dt) {
    float dtSec = dt.asSeconds();
    m_hudManager.updateTimer(dtSec);
    if (m_hudManager.getRemainingTime() <= 0.0f) {
        // Time out
    }


    // Update hero, blocks, items
    if (m_hero) {
       m_hero->update(dtSec);
        int currentCoins = m_hero->getCoin();
        if (currentCoins > m_lastCoinCount) {
            int diff = currentCoins - m_lastCoinCount;
            m_hudManager.addCoin(diff);
            m_hudManager.addScore(100 * diff);
            m_lastCoinCount = currentCoins;
        }
    }
    for (size_t i = 0; i < m_blocks.size(); ++i) m_blocks[i]->update(dtSec);
    for (size_t i = 0; i < m_items.size(); ++i) m_items[i]->update(dtSec);

    if (!m_hero->isDead()){
    // 2. Predict newpos of hero
        sf::Vector2f oldpos=m_hero->getPosition();
        sf::Vector2f vel=m_hero->getVelocity();

        // Check collisions for X-axis first
        m_hero->setPosition(oldpos.x+vel.x*dtSec, oldpos.y);
        m_physics.resolveCollisionX(m_hero->getHitbox(), m_dummyFloor, vel.x);
        m_physics.resolveCollisionX(m_hero->getHitbox(), m_dummyWall, vel.x);
        for (size_t i=0;i<m_blocks.size();++i){
            if (m_blocks[i]->getIsActive()){
                m_physics.resolveCollisionX(m_hero->getHitbox(), m_blocks[i]->getHitbox(), vel.x);
            }
        }

        //Checkcollisions for Y-axis
        // Lấy lại vị trí X đã được resolve từ hitbox, nếu không X collision sẽ bị mất!
        m_hero->setPosition(m_hero->getHitbox().getPosition().x, oldpos.y+vel.y*dtSec);
        bool grounded=false;
        if (m_physics.checkCollision(m_hero->getHitbox(),m_dummyFloor)==SideType::Top) grounded=true;
        m_physics.resolveCollisionY(m_hero->getHitbox(), m_dummyFloor, vel.y);
        if (m_physics.checkCollision(m_hero->getHitbox(),m_dummyWall)==SideType::Top) grounded=true;
        m_physics.resolveCollisionY(m_hero->getHitbox(), m_dummyWall, vel.y);
        for (size_t i=0;i<m_blocks.size();++i){
            if (m_blocks[i]->getIsActive()){
                if (m_physics.checkCollision(m_hero->getHitbox(),m_blocks[i]->getHitbox())==SideType::Top) grounded=true;
                
                float oldVelY = vel.y;
                m_physics.resolveCollisionY(m_hero->getHitbox(), m_blocks[i]->getHitbox(), vel.y);
                
                if (vel.y == 0.f && oldVelY < 0.f) {
                    if (auto spawnedItem = m_blocks[i]->hit(hero.get())) {
                        m_items.push_back(std::move(spawnedItem));   
                    }
                }
            }
        }

        m_hero->setGrounded(grounded);
        sf::Vector2f pos=m_hero->getHitbox().getPosition();
        m_hero->setPosition(pos.x,pos.y);
        m_hero->setVelocity(vel.x,vel.y);
    }

    for (auto& item:m_items ){
        sf::Vector2f oldpos=item->getPosition();
        sf::Vector2f vel=item->getVelocity();
        if (item->isColliable()){
            //Check collision in X-axis first
            float oldVelX = vel.x;
            item->setPosition(oldpos.x+vel.x*dtSec, oldpos.y);
            m_physics.resolveCollisionX(item->getHitbox(), m_dummyFloor, vel.x);
            m_physics.resolveCollisionX(item->getHitbox(), m_dummyWall, vel.x);
            for (size_t i=0;i<m_blocks.size();++i){
               if (m_blocks[i]->getIsActive()){
                   m_physics.resolveCollisionX(item->getHitbox(), m_blocks[i]->getHitbox(), vel.x);
                }
            }

            // Bounce logic
            if (oldVelX != 0.f && vel.x == 0.f) {
                item->getCollision(); // Reverses velocity in Mushroom/Star
                vel = item->getVelocity(); // Refresh vel after getCollision
            }

            //Checkcollisions for Y-axis
            // Lấy lại vị trí X đã được resolve từ hitbox, nếu không X collision sẽ bị mất!
            item->setPosition(item->getHitbox().getPosition().x, oldpos.y+vel.y*dtSec);
            bool grounded=false;
            if (m_physics.checkCollision(item->getHitbox(),m_dummyFloor)==SideType::Top) grounded=true;
            m_physics.resolveCollisionY(item->getHitbox(), m_dummyFloor, vel.y);
            if (m_physics.checkCollision(item->getHitbox(),m_dummyWall)==SideType::Top) grounded=true;
            m_physics.resolveCollisionY(item->getHitbox(), m_dummyWall, vel.y);
            for (size_t i=0;i<m_blocks.size();++i){
                if (m_blocks[i]->getIsActive()){
                    if (m_physics.checkCollision(item->getHitbox(),m_blocks[i]->getHitbox())==SideType::Top) grounded=true;
                    m_physics.resolveCollisionY(item->getHitbox(), m_blocks[i]->getHitbox(), vel.y);
                }
            }

            item->setGrounded(grounded);
            sf::Vector2f pos=item->getHitbox().getPosition();
            item->setPosition(pos.x,pos.y);
            item->setVelocity(vel.x,vel.y);
        }
        else {
            
            // If item is not collidable, we just update its position based on velocity
            item->setPosition(oldpos.x + vel.x * dtSec, oldpos.y + vel.y * dtSec);
        }
    }

    // Interact with items
    for (auto& item : m_items) {
        if (item->isColliable() && m_physics.checkCollision(m_hero->getHitbox(), item->getHitbox()) != SideType::None) {
            m_hero->collectItem(item.get());
        }
    }

    // Update enemies
    for (auto it = m_enemies.begin(); it != m_enemies.end();) {
        (*it)->update(dt.asSeconds());
        
        if ((*it)->getIsAlive() && (*it)->getStateName() != "FlippingDeath" && (*it)->getStateName() != "Squished" && m_hero && !m_hero->isDead() && (*it)->getBounds().intersects(m_hero->getBounds())) {
            sf::FloatRect enemyBounds = (*it)->getBounds();
            sf::FloatRect heroBounds = m_hero->getBounds();

            bool isFallingInAir = (!m_hero->getIsGrounded() && m_hero->getVelocity().y >= 0.f);
            float marioBottomY = heroBounds.top + heroBounds.height;
            float enemyTopY = enemyBounds.top;

            // Stomp logic: Mario must be falling from the air and hit the top half of the enemy
            if (isFallingInAir && marioBottomY <= enemyTopY + (enemyBounds.height * 0.6f)) {
                (*it)->onStomped(nullptr);
                m_hero->setVelocity(hero->getVelocity().x, -300.f); // Bounce Mario up!
                m_hudManager.addScore(200);
            } else {
                (*it)->onSideCollision(nullptr);
                if ((*it)->getDamageOnTouch() > 0) {
                    m_hero->takedamage();
                }
            }
        }

        if (!(*it)->getIsAlive()) {
            it = m_enemies.erase(it);
        } else {
            ++it;
        }
    }

    for (auto it = m_projectiles.begin(); it != m_projectiles.end();) {
        (*it)->update(dt.asSeconds());
        if ((*it)->getIsAlive() && m_hero && !m_hero->isDead() && (*it)->getBounds().intersects(m_hero->getBounds())) {
            (*it)->die();
            m_hero->takedamage();
        }
        if (!(*it)->getIsAlive()) {
            it = m_projectiles.erase(it);
        } else {
            ++it;
        }
    }

    //Clear block and item if they are not active
    m_blocks.erase(std::remove_if(blocks.begin(), blocks.end(),
        [](const std::unique_ptr<Block>& block) { return !block->getIsActive(); }),
        blocks.end());
    m_items.erase(std::remove_if(items.begin(), items.end(),
        [](const std::unique_ptr<Item>& item) { return item->isCollected(); }),
        items.end());
}
    

void PlayingState::render(sf::RenderWindow& window) {
    window.draw(m_levelManager);

    window.draw(m_dummyFloor);

    window.draw(m_dummyWall);
    for (auto& block : m_blocks) block->render(window);
    for (auto& item : m_items) item->render(window);
    if (m_hero) m_hero->render(window);

    for (auto& enemy : m_enemies) {
        if (enemy) {
            enemy->render(window);
        }
    }

    for (auto& projectile : m_projectiles) {
        if (projectile) {
            projectile->render(window);
        }
    }
    window.draw(m_hudManager);
	window.setView(window.getDefaultView()); // Reset view to default for UI rendering
}