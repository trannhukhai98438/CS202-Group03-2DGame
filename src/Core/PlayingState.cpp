#include "Core/PlayingState.h"
#include "Core/Game.h"
#include "Core/PausedState.h"
#include "Core/GameOverState.h"
#include "Core/VictoryState.h"
#include "Entities/Character/Enemy/Potion.h"
#include "Entities/Character/Enemy/EnemyState.h"
#include <iostream>


PlayingState::PlayingState(): m_physics(), m_hudManager(), m_lastCoinCount(0) {
    m_camera.setSize(1280.f, 720.f);
    m_hudManager.init("assets/fonts/SuperMario256.ttf");
    std::string jsonPath = "assets/maps/levels/1-1.tmj";
    std::string tilesetPath = "assets/maps/resources/tileset.png";
    if (!m_levelManager.loadLevel(jsonPath, tilesetPath)) {
        std::cerr << "[PlayingState] ERROR: Cannot load level!" << std::endl;
    }
    MapObject spawnPoint;
    if (m_levelManager.getObjectByName("Objects", "SpawnPoint", spawnPoint)) {
        m_hero = HeroFactory().createHero(HeroType::Luigi, spawnPoint.x * 2.f, spawnPoint.y * 2.f + 272.f);
    } else {
        m_hero = HeroFactory().createHero(HeroType::Luigi, 100.f, 500.f);
    }

    // Build map colliders from Terrain layer
    int tileW = m_levelManager.getTileWidth();
    int tileH = m_levelManager.getTileHeight();
    for (int y = 0; y < m_levelManager.getMapHeightTiles(); ++y) {
        for (int x = 0; x < m_levelManager.getMapWidthTiles(); ++x) {
            if (m_levelManager.isSolidAtTile(x, y)) {
                sf::RectangleShape rect;
                rect.setSize({ tileW * 2.f, tileH * 2.f });
                rect.setPosition(x * tileW * 2.f, y * tileH * 2.f + 272.f);
                m_mapColliders.push_back(rect);
            }
        }
    }

    auto spawnCallback = [this](std::unique_ptr<Projectile> p) {
        m_projectiles.push_back(std::move(p));
    };

    m_enemies.push_back(EnemyFactory::createEnemy(EnemyType::Goomba, 300.f, 624.f, 150.f, spawnCallback));
    m_enemies.push_back(EnemyFactory::createEnemy(EnemyType::Koopa, 600.f, 608.f, 200.f, spawnCallback));
    m_enemies.push_back(EnemyFactory::createEnemy(EnemyType::Witch, 900.f, 560.f, 150.f, spawnCallback));
    
    // We can disable hardcoded blocks or move them to match the new grid (e.g. y = 464.f)
    // BlockFactory blockFac;
    // m_blocks.push_back(blockFac.createBlock(BlockType::Brick, 300, 464.f));
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
        static float timer = 0.0f;
        timer += dtSec;
        if (timer >= 1.0f) {
            timer = 0.0f;
            std::cout << "Hero Y: " << m_hero->getPosition().y << std::endl;
            for (auto& enemy : m_enemies) {
                std::cout << "Enemy type " << (int)enemy->getBounds().height << " Y: " << enemy->getPosition().y << std::endl;
            }
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
        for (auto& collider : m_mapColliders) {
            m_physics.resolveCollisionX(m_hero->getHitbox(), collider, vel.x);
        }
        for (size_t i=0;i<m_blocks.size();++i){
            if (m_blocks[i]->getIsActive()){
                m_physics.resolveCollisionX(m_hero->getHitbox(), m_blocks[i]->getHitbox(), vel.x);
            }
        }

        //Checkcollisions for Y-axis
        // Lấy lại vị trí X đã được resolve từ hitbox, nếu không X collision sẽ bị mất!
        m_hero->setPosition(m_hero->getHitbox().getPosition().x, oldpos.y+vel.y*dtSec);
        bool grounded=false;
        for (auto& collider : m_mapColliders) {
            if (m_physics.checkCollision(m_hero->getHitbox(), collider) == SideType::Top) grounded = true;
            m_physics.resolveCollisionY(m_hero->getHitbox(), collider, vel.y);
        }
        for (size_t i=0;i<m_blocks.size();++i){
            if (m_blocks[i]->getIsActive()){
                if (m_physics.checkCollision(m_hero->getHitbox(),m_blocks[i]->getHitbox())==SideType::Top) grounded=true;
                
                float oldVelY = vel.y;
                m_physics.resolveCollisionY(m_hero->getHitbox(), m_blocks[i]->getHitbox(), vel.y);
                
                if (vel.y == 0.f && oldVelY < 0.f) {
                    if (auto spawnedItem = m_blocks[i]->hit(m_hero.get())) {
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
            for (auto& collider : m_mapColliders) {
                m_physics.resolveCollisionX(item->getHitbox(), collider, vel.x);
            }
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
            for (auto& collider : m_mapColliders) {
                if (m_physics.checkCollision(item->getHitbox(), collider) == SideType::Top) grounded = true;
                m_physics.resolveCollisionY(item->getHitbox(), collider, vel.y);
            }
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
        
        // Apply gravity and Y-collision
        if ((*it)->getIsAlive() && (*it)->getStateName() != "FlippingDeath" && (*it)->getStateName() != "Squished") {
            sf::Vector2f oldpos = (*it)->getPosition();
            sf::Vector2f vel = (*it)->getVelocity();
            vel.y += 1500.0f * dt.asSeconds(); // Gravity
            
            // X-collision with map walls/pipes -> turn around!
            bool hitWall = false;
            for (auto& collider : m_mapColliders) {
                SideType side = m_physics.checkCollision((*it)->getHitbox(), collider);
                if (side == SideType::Left || side == SideType::Right) {
                    hitWall = true;
                }
                float dummyVelX = static_cast<float>((*it)->getDirection()) * (*it)->getSpeed();
                m_physics.resolveCollisionX((*it)->getHitbox(), collider, dummyVelX);
            }
            if (hitWall) {
                (*it)->flipDirection();
            }

            (*it)->setPosition(sf::Vector2f((*it)->getHitbox().getPosition().x, oldpos.y + vel.y * dt.asSeconds()));
            
            for (auto& collider : m_mapColliders) {
                m_physics.resolveCollisionY((*it)->getHitbox(), collider, vel.y);
            }
            
            (*it)->setVelocity(vel.x, vel.y);
            (*it)->setPosition((*it)->getHitbox().getPosition());
        }

        // Handle spinning shell vs other enemies (swept combo chain)
        if ((*it)->getIsAlive() && (*it)->getStateName() == "SpinningShell") {
            for (auto& otherEnemy : m_enemies) {
                if (otherEnemy->getIsAlive() && otherEnemy.get() != it->get() && 
                    otherEnemy->getStateName() != "FlippingDeath" && otherEnemy->getStateName() != "Squished") {
                    if ((*it)->getBounds().intersects(otherEnemy->getBounds())) {
                        otherEnemy->changeState(std::make_unique<FlippingDeathState>(-300.0f));
                        m_hudManager.addScore(200);
                    }
                }
            }
        }

        if ((*it)->getIsAlive() && (*it)->getStateName() != "FlippingDeath" && (*it)->getStateName() != "Squished" && m_hero && !m_hero->isDead() && (*it)->getBounds().intersects(m_hero->getBounds())) {
            int scoreEarned = m_hero->interactWith(it->get());
            if (scoreEarned > 0) {
                m_hudManager.addScore(scoreEarned);
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
        
        // Potion shatter or puddle Y-collision on collision with map tiles
        for (auto& collider : m_mapColliders) {
            if ((*it)->getBounds().intersects(collider.getGlobalBounds())) {
                if (auto potion = dynamic_cast<Potion*>(it->get())) {
                    if (!potion->getIsPuddle()) {
                        std::cout << "[Debug] Potion shattered by collider at X=" << collider.getPosition().x 
                                  << ", Y=" << collider.getPosition().y << std::endl;
                        potion->shatterOnTile(collider.getPosition().y);
                    } else {
                        float velY = potion->getVelocity().y;
                        m_physics.resolveCollisionY(potion->getHitbox(), collider, velY);
                        potion->setPosition(potion->getHitbox().getPosition());
                        potion->setVelocity(potion->getVelocity().x, velY);
                    }
                }
            }
        }

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
    if (m_hero->getPosition().y> 720.f) {
        m_hero->die();
    }
    //Clear block and item if they are not active
    m_blocks.erase(std::remove_if(m_blocks.begin(), m_blocks.end(),
        [](const std::unique_ptr<Block>& block) { return !block->getIsActive(); }),
        m_blocks.end());
    m_items.erase(std::remove_if(m_items.begin(), m_items.end(),
        [](const std::unique_ptr<Item>& item) { return item->isCollected(); }),
        m_items.end());
    float marioX = m_hero->getPosition().x;
    float halfScreenWidth = 640.f;
    float levelEnd = 5000.f; //Wherever the level ends. This is just a PLACEHOLDER for now.
    float cameraX = std::clamp(marioX, halfScreenWidth, levelEnd - halfScreenWidth);
    m_camera.setCenter(cameraX, 360.f);
	// TEST SCREENS (delete this when we have a proper Mario sprite and level assets)
    // Press 'L' to simulate Mario dying
    if (m_hero->isDead()) {
        Game::getInstance().changeState(std::make_unique<GameOverState>());
    }

    // Press 'W' to simulate touching the flagpole
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        Game::getInstance().changeState(std::make_unique<VictoryState>());
    }
}

void PlayingState::render(sf::RenderWindow& window) {
    window.setView(m_camera);
    window.draw(m_levelManager);

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