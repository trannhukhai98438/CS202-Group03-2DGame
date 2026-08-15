#include "Core/PlayingState.h"
#include "Core/Game.h"
#include "Core/PausedState.h"
#include "Core/GameOverState.h"
#include "Core/VictoryState.h"
#include "Entities/Character/Enemy/EnemyState.h"
#include "Entities/Character/Hero/HeroState/FlyState.h"
#include "Entities/Goal/Flag.h"
#include "Entities/Item/Coin.h"
#include <iostream>
#include <unordered_set>

namespace {
constexpr float VICTORY_DELAY_SECONDS = 0.75f;
}

PlayingState::PlayingState(): m_physics(), m_hudManager(), m_lastCoinCount(0) {
    m_camera.setSize(1280.f, 720.f);
    m_hudManager.init("assets/fonts/SuperMario256.ttf");
    std::string jsonPath = "assets/maps/levels/1-1.tmj";
    std::string tilesetPath = "assets/maps/resources/tileset.png";
    if (!m_levelManager.loadLevel(jsonPath, tilesetPath)) {
        std::cerr << "[PlayingState] ERROR: Cannot load level!" << std::endl;
    }
    auto spawnCallback = [this](std::unique_ptr<Projectile> projectile) {
        if (projectile) m_projectiles.push_back(std::move(projectile));
    };
    constexpr HeroType selectedHero = HeroType::Mario; // Change here to test Mario.
    MapObject spawnPoint;
    if (m_levelManager.getObjectByName("Objects", "SpawnPoint", spawnPoint)) {
        m_hero = HeroFactory().createHero(selectedHero,
            spawnPoint.x * 2.f, spawnPoint.y * 2.f + 272.f, spawnCallback);
    } else {
        m_hero = HeroFactory().createHero(selectedHero, 100.f, 500.f,
                                          spawnCallback);
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

    m_enemies.push_back(EnemyFactory::createEnemy(EnemyType::Goomba, 300.f, 624.f, 150.f, spawnCallback));
    m_enemies.push_back(EnemyFactory::createEnemy(EnemyType::Koopa, 600.f, 608.f, 200.f, spawnCallback));
    m_enemies.push_back(EnemyFactory::createEnemy(EnemyType::Witch, 900.f, 560.f, 150.f, spawnCallback));
    
    // ---- Spawn Interactive blocks & ground coins from map ObjectLayer ----
    // Coordinate mapping:
    //   world X = obj.x * 2          (Tiled 16px grid -> 32px world)
    //   world Y = obj.y * 2 + 272    (same scale, plus 272px draw offset)
    //   If tile object (gid>0): obj.y is BOTTOM of object in Tiled coords.
    {
        BlockFactory blockFac;
        auto objs = m_levelManager.getObjectsFromLayer("Interactive");
        for (const auto& obj : objs) {
            float wx = obj.x * 2.f;
            float wy = (obj.gid > 0)
                       ? (obj.y - obj.height) * 2.f + 272.f // tile obj: y=bottom
                       :  obj.y * 2.f + 272.f;              // rect obj: y=top

            const std::string& type = obj.className;

            if (type == "brick") {
                m_blocks.push_back(blockFac.createBlock(BlockType::Brick, wx, wy));

            } else if (type == "question") {
                ItemType itype = ItemType::Coin;
                std::string item = obj.getProperty("item", "coin");
                if (item == "mushroom" || item == "flower")
                    itype = ItemType::PowerUpPrototype;
                else if (item == "star")
                    itype = ItemType::Star;
                m_blocks.push_back(blockFac.createBlock(BlockType::Question, wx, wy, itype));

            } else if (type == "invisible") {
                // Invisible blocks always contain mushroom/powerup
                m_blocks.push_back(blockFac.createBlock(BlockType::Question, wx, wy,
                                                         ItemType::PowerUpPrototype));

            } else if (type == "coin") {
                // Floating ground coin — spawned as an Item directly
                auto coin = std::make_unique<Coin>(wx, wy);
                coin->spawnAsGroundCoin();
                m_items.push_back(std::move(coin));

            } else if (type == "flag") {
                sf::FloatRect triggerBounds(
                    wx,
                    wy,
                    obj.width * 2.f,
                    obj.height * 2.f);
                m_levelGoals.push_back(
                    std::make_unique<Flag>(triggerBounds));
            }
        }
    }

    // ---- Hardcoded blocks (kept as reference, disabled) ----
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
        if (item->isColliable()) {
            // Collidable items (mushroom, star): use hitbox collision
            if (m_physics.checkCollision(m_hero->getHitbox(), item->getHitbox()) != SideType::None) {
                m_hero->collectItem(item.get());
            }
        } else {
            // Non-collidable items (coins): simple bounds overlap
            if (m_hero->getBounds().intersects(item->getBounds())) {
                m_hero->collectItem(item.get());
            }
        }
    }

    // Update enemies
    for (auto it = m_enemies.begin(); it != m_enemies.end();) {
        (*it)->update(dt.asSeconds());

        // Clean up enemies that fall off cliffs/into pits below the screen
        if ((*it)->getPosition().y > 800.0f) {
            (*it)->die();
        }
        
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
    updateProjectiles(dtSec);
    if (m_hero->getPosition().y> 720.f) {
        m_hero->takeDamage(1);
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
    // Map world width = tile count * tile size (16px) * 2 (render scale)
    float levelEnd = static_cast<float>(m_levelManager.getMapWidthPixels()) * 2.f;
    float cameraX = std::clamp(marioX, halfScreenWidth, levelEnd - halfScreenWidth);
    m_camera.setCenter(cameraX, 360.f);
    if (!m_victoryPending && m_hero && !m_hero->isDead()) {
        for (auto& goal : m_levelGoals) {
            GoalResult result = goal->tryActivate(*m_hero);
            if (!result.activated) continue;
            m_hero->setState(std::make_unique<FlyState>());
            m_hudManager.addScore(result.scoreAwarded);
            m_victoryPending = true;
            m_victoryDelayRemaining = VICTORY_DELAY_SECONDS;
            break;
        }
    } else if (m_victoryPending) {
        m_victoryDelayRemaining -= dtSec;
        if (m_victoryDelayRemaining <= 0.0f) {
            Game::getInstance().changeState(std::make_unique<VictoryState>());
            return;
        }
    }

    // Victory remains locked even if the continuing simulation kills the
    // hero during the short transition delay.
    if (!m_victoryPending && m_hero->isDead()) {
        Game::getInstance().changeState(std::make_unique<GameOverState>());
        return;
    }
}

void PlayingState::simulateProjectile(Projectile& projectile, float deltaTime) {
    if (!projectile.getIsAlive() || !projectile.usesWorldPhysics()) return;

    sf::Vector2f velocity = projectile.getVelocity();
    m_physics.applyGravity(velocity.y, deltaTime,
                           projectile.getGravityAcceleration());
    projectile.setVelocity(velocity);

    auto forEachSolid = [this](auto&& visitor) {
        for (const auto& collider : m_mapColliders) {
            if (!visitor(collider.getGlobalBounds())) return;
        }
        for (const auto& block : m_blocks) {
            if (block->getIsActive()
                && !visitor(block->getHitbox().getGlobalBounds())) return;
        }
    };

    sf::Vector2f oldPosition = projectile.getPosition();
    projectile.setPosition({oldPosition.x + velocity.x * deltaTime,
                            oldPosition.y});

    forEachSolid([&](const sf::FloatRect& solidBounds) {
        SideType side = m_physics.checkCollision(projectile.getBounds(),
                                                  solidBounds);
        if (side != SideType::Left && side != SideType::Right) return true;

        m_physics.resolveCollisionX(projectile.getHitbox(), solidBounds,
                                    velocity.x);
        projectile.setPosition(projectile.getHitbox().getPosition());
        projectile.setVelocity(velocity);
        projectile.onSolidCollision(side, solidBounds);
        velocity = projectile.getVelocity();
        return projectile.getIsAlive() && projectile.usesWorldPhysics();
    });

    if (!projectile.getIsAlive() || !projectile.usesWorldPhysics()) return;

    oldPosition = projectile.getPosition();
    projectile.setPosition({oldPosition.x,
                            oldPosition.y + velocity.y * deltaTime});

    forEachSolid([&](const sf::FloatRect& solidBounds) {
        SideType side = m_physics.checkCollision(projectile.getBounds(),
                                                  solidBounds);
        if (side == SideType::None) return true;

        if (side == SideType::Top || side == SideType::Bottom) {
            m_physics.resolveCollisionY(projectile.getHitbox(), solidBounds,
                                        velocity.y);
        } else {
            m_physics.resolveCollisionX(projectile.getHitbox(), solidBounds,
                                        velocity.x);
        }
        projectile.setPosition(projectile.getHitbox().getPosition());
        projectile.setVelocity(velocity);
        projectile.onSolidCollision(side, solidBounds);
        velocity = projectile.getVelocity();
        return projectile.getIsAlive() && projectile.usesWorldPhysics();
    });

    projectile.setVelocity(velocity);
}

void PlayingState::resolveProjectileTargets(Projectile& projectile) {
    if (!projectile.getIsAlive()) return;

    if (projectile.getFaction() == ProjectileFaction::Enemy) {
        if (m_hero && !m_hero->isDead()
            && projectile.getBounds().intersects(m_hero->getBounds())) {
            projectile.onHitTarget(*m_hero);
        }
        projectile.onTargetResolutionComplete();
        return;
    }

    // A projectile may expand its target bounds on impact. The second pass
    // catches targets that appeared earlier in the list; the set guarantees
    // one resolution per target in this frame.
    std::unordered_set<Enemy*> resolvedTargets;
    for (int pass = 0; pass < 2 && projectile.getIsAlive(); ++pass) {
        for (auto& enemy : m_enemies) {
            if (!projectile.getIsAlive()) break;
            if (!enemy->getIsAlive()
                || enemy->getStateName() == "FlippingDeath"
                || enemy->getStateName() == "Squished"
                || resolvedTargets.find(enemy.get()) != resolvedTargets.end()) {
                continue;
            }
            if (!projectile.getBounds().intersects(enemy->getBounds())) continue;

            resolvedTargets.insert(enemy.get());
            if (projectile.onHitTarget(*enemy)) {
                m_hudManager.addScore(enemy->getScoreValue());
            }
        }
    }
    projectile.onTargetResolutionComplete();
}

void PlayingState::updateProjectiles(float deltaTime) {
    const float worldWidth = m_levelManager.getMapWidthPixels() * 2.0f;

    for (auto it = m_projectiles.begin(); it != m_projectiles.end();) {
        Projectile& projectile = **it;
        projectile.update(deltaTime);
        simulateProjectile(projectile, deltaTime);
        resolveProjectileTargets(projectile);

        sf::FloatRect bounds = projectile.getBounds();
        if (bounds.left + bounds.width < 0.0f
            || bounds.left > worldWidth
            || bounds.top > 900.0f) {
            projectile.die();
        }

        if (!projectile.getIsAlive()) it = m_projectiles.erase(it);
        else ++it;
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
