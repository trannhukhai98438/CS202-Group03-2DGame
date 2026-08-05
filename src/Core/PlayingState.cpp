#include "Core/PlayingState.h"
#include "Core/Game.h"
#include "Core/PausedState.h"
#include "Entities/Character/Hero/HeroFactory.h"
#include "Entities/Block/BlockFactory.h"
#include "Entities/Item/ItemFactory.h"
#include <algorithm>


PlayingState::PlayingState(){
    hero=HeroFactory().createHero(HeroType::Luigi,600,100);

    m_dummyFloor.setSize(sf::Vector2f(800.f, 50.f));
    m_dummyFloor.setFillColor(sf::Color::Green);
    m_dummyFloor.setPosition(240.f, 600.f);

    m_dummyWall.setSize(sf::Vector2f(50.f, 200.f));
    m_dummyWall.setFillColor(sf::Color::Magenta);
    m_dummyWall.setPosition(800.f, 400.f);

    BlockFactory blockFac;
    blocks.push_back(blockFac.createBlock(BlockType::Brick, 300,550));
    blocks.push_back(blockFac.createBlock(BlockType::Question,400, 550, ItemType::PowerUpPrototype));
    blocks.push_back(blockFac.createBlock(BlockType::Question,420, 550, ItemType::PowerUpPrototype));
    blocks.push_back(blockFac.createBlock(BlockType::Question,440, 550, ItemType::Coin));
    blocks.push_back(blockFac.createBlock(BlockType::Question,460, 550, ItemType::Star));
}

PlayingState::~PlayingState() = default;

void PlayingState::processEvents(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab) {
        Game::getInstance().pushState(std::make_unique<PausedState>());
    }
    // key-input will be processed in update(dt)
}

void PlayingState::update(sf::Time dt) {
    float dtSec = dt.asSeconds();

    // 1. Update entities (this handle velocity and gravity)
    hero->update(dtSec);
    for (size_t i=0;i<blocks.size();++i) blocks[i]->update(dtSec);
    for (size_t i=0;i<items.size();++i) items[i]->update(dtSec);

    if (!hero->isDead()){
    // 2. Predict newpos of hero
        sf::Vector2f oldpos=hero->getPosition();
        sf::Vector2f vel=hero->getVelocity();

        // Check collisions for X-axis first
        hero->setPosition(oldpos.x+vel.x*dtSec, oldpos.y);
        m_physics.resolveCollisionX(hero->getHitbox(), m_dummyFloor, vel.x);
        m_physics.resolveCollisionX(hero->getHitbox(), m_dummyWall, vel.x);
        for (size_t i=0;i<blocks.size();++i){
            if (blocks[i]->getIsActive()){
                m_physics.resolveCollisionX(hero->getHitbox(), blocks[i]->getHitbox(), vel.x);
            }
        }

        //Checkcollisions for Y-axis
        // Lấy lại vị trí X đã được resolve từ hitbox, nếu không X collision sẽ bị mất!
        hero->setPosition(hero->getHitbox().getPosition().x, oldpos.y+vel.y*dtSec);
        bool grounded=false;
        if (m_physics.checkCollision(hero->getHitbox(),m_dummyFloor)==SideType::Top) grounded=true;
        m_physics.resolveCollisionY(hero->getHitbox(), m_dummyFloor, vel.y);
        if (m_physics.checkCollision(hero->getHitbox(),m_dummyWall)==SideType::Top) grounded=true;
        m_physics.resolveCollisionY(hero->getHitbox(), m_dummyWall, vel.y);
        for (size_t i=0;i<blocks.size();++i){
            if (blocks[i]->getIsActive()){
                if (m_physics.checkCollision(hero->getHitbox(),blocks[i]->getHitbox())==SideType::Top) grounded=true;
                
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
        sf::Vector2f pos=hero->getHitbox().getPosition();
        hero->setPosition(pos.x,pos.y);
        hero->setVelocity(vel.x,vel.y);
    }

    for (auto& item:items ){
        sf::Vector2f oldpos=item->getPosition();
        sf::Vector2f vel=item->getVelocity();
        if (item->isColliable()){
            //Check collision in X-axis first
            float oldVelX = vel.x;
            item->setPosition(oldpos.x+vel.x*dtSec, oldpos.y);
            m_physics.resolveCollisionX(item->getHitbox(), m_dummyFloor, vel.x);
            m_physics.resolveCollisionX(item->getHitbox(), m_dummyWall, vel.x);
            for (size_t i=0;i<blocks.size();++i){
               if (blocks[i]->getIsActive()){
                   m_physics.resolveCollisionX(item->getHitbox(), blocks[i]->getHitbox(), vel.x);
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
            for (size_t i=0;i<blocks.size();++i){
                if (blocks[i]->getIsActive()){
                    if (m_physics.checkCollision(item->getHitbox(),blocks[i]->getHitbox())==SideType::Top) grounded=true;
                    m_physics.resolveCollisionY(item->getHitbox(), blocks[i]->getHitbox(), vel.y);
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
    for (auto& item : items) {
        if (item->isColliable() && m_physics.checkCollision(hero->getHitbox(), item->getHitbox()) != SideType::None) {
            hero->collectItem(item.get());
        }
    }

    //Clear block and item if they are not active
    blocks.erase(std::remove_if(blocks.begin(), blocks.end(),
        [](const std::unique_ptr<Block>& block) { return !block->getIsActive(); }),
        blocks.end());
    items.erase(std::remove_if(items.begin(), items.end(),
        [](const std::unique_ptr<Item>& item) { return item->isCollected(); }),
        items.end());
}
    

void PlayingState::render(sf::RenderWindow& window) {
    window.draw(m_dummyFloor);
    window.draw(m_dummyWall);
    for (auto& block : blocks) block->render(window);
    for (auto& item : items) item->render(window);
    if (hero) hero->render(window);
}