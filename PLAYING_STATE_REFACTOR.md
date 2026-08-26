# PlayingState Refactor Guide

This document explains the current gameplay architecture after the `PlayingState`
refactor. Its purpose is to help team members locate the correct code when adding a
feature, investigating a bug, or changing another member's subsystem.

The descriptions below reflect the current implementation. Items marked as
**temporary** or **known limitation** are not architectural promises.

## Why `PlayingState` Was Refactored

Previously, `PlayingState` was responsible for loading the map, constructing every
entity, updating entities, resolving physics, resolving gameplay interactions,
removing dead objects, rendering the world, updating the HUD, controlling the
camera, and changing game states.

That made unrelated gameplay changes converge on one large file and made it easy
to introduce ordering bugs. The refactor keeps `PlayingState` as the scene-level
coordinator while moving world responsibilities behind `LevelRuntime`.

```text
PlayingState
 |-- camera and HUD
 |-- pause input
 |-- victory and defeat transitions
 `-- LevelRuntime                         gameplay facade
      |-- GameWorld                       ownership and collections
      |-- WorldPhysicsSystem              movement and solid collision
      `-- InteractionSystem               entity-to-entity interactions

LevelBuilder                              constructs GameWorld once at startup
```

## Source File Map

| Responsibility | Header | Implementation |
|---|---|---|
| Scene coordination | `include/Core/PlayingState.h` | `src/Core/PlayingState.cpp` |
| Runtime facade and frame pipeline | `include/Core/Gameplay/LevelRuntime.h` | `src/Core/Gameplay/LevelRuntime.cpp` |
| World ownership | `include/Core/Gameplay/GameWorld.h` | `src/Core/Gameplay/GameWorld.cpp` |
| Map and entity construction | `include/Core/Gameplay/LevelBuilder.h` | `src/Core/Gameplay/LevelBuilder.cpp` |
| Terrain and block collision | `include/Core/Gameplay/WorldPhysicsSystem.h` | `src/Core/Gameplay/WorldPhysicsSystem.cpp` |
| Entity interactions | `include/Core/Gameplay/InteractionSystem.h` | `src/Core/Gameplay/InteractionSystem.cpp` |

## Responsibilities and Ownership

### `PlayingState`

`PlayingState` owns presentation and scene transitions:

- the camera;
- the HUD;
- pause input;
- score and coin synchronization with the current HUD implementation;
- the victory delay and transition to `VictoryState`;
- the defeat delay and transition to `GameOverState`;
- one `LevelRuntime` instance.

It must not parse map objects, own entity containers, or contain entity-pair
collision loops. A feature that changes world simulation should normally be placed
in one of the runtime classes instead.

### `LevelRuntime`

`LevelRuntime` is the facade used by `PlayingState`. It owns the running
`GameWorld`, `WorldPhysicsSystem`, and `InteractionSystem`, and exposes only the
operations needed by the scene:

```cpp
int update(float deltaTime);
void renderWorld(sf::RenderWindow& window);
Hero* getHero();
float getWorldWidth() const;
bool hasActivatedGoal() const;
bool isReady() const;
```

It defines the update and rendering order. If a new entity category is introduced,
this is where its intrinsic `update()` and `render()` calls must be added.

### `GameWorld`

`GameWorld` is the owner of level data and runtime entities:

- `LevelManager`;
- the Hero;
- blocks;
- items;
- enemies;
- projectiles;
- level goals;
- terrain colliders generated from solid map tiles.

Ownership uses `std::unique_ptr`. Systems receive a `GameWorld&` and operate on
its collections without taking ownership.

`GameWorld::removeInactiveEntities()` is the single cleanup point for blocks,
items, enemies, and projectiles. During update or interaction loops, mark an object
inactive, collected, or dead. Do not erase it from its vector while iterating.

### `LevelBuilder`

`LevelBuilder` is used once by the `LevelRuntime` constructor. It:

1. loads the map and tileset through `LevelManager`;
2. creates the projectile-spawn callback;
3. creates the selected Hero at `Objects/SpawnPoint`, or at the fallback position;
4. converts solid map tiles into world colliders;
5. reads the `Interactive` object layer;
6. creates blocks, items, and goals through their factories;
7. creates the three temporary enemies.

The current map-to-world transform is:

```text
world X = map X * 2
world Y = map Y * 2 + 272
```

For Tiled tile objects (`gid > 0`), the object's Y value is its bottom edge, so
the builder first subtracts `object.height`.

The three enemies at the end of `LevelBuilder::build()` are temporary. Replace
that block when an `Enemies` object layer is added to the maps.

## Initialization Flow

```text
PlayingState constructor
 `-- constructs LevelRuntime(map path, tileset path, HeroType)
      `-- LevelBuilder::build(GameWorld, ...)
           |-- LevelManager::loadLevel(...)
           |-- HeroFactory::createHero(...)
           |-- build terrain colliders
           |-- BlockFactory / ItemFactory / Flag construction
           `-- temporary EnemyFactory calls
```

The map path, tileset path, and `HeroType::Mario` are currently selected directly
in the `PlayingState` constructor.

The projectile callback captures the `GameWorld` owned by the same runtime. Hero
and Witch projectile creation therefore calls `GameWorld::addProjectile()` without
giving the entity direct access to the complete world.

## Per-Frame Update Order

Update order is gameplay behavior. Changing it can create one-frame differences,
missed collisions, or interactions with objects that should already be dead.

### Scene update

`PlayingState::update()` currently performs these steps:

```text
1. Update the HUD timer
2. LevelRuntime::update(deltaTime)
3. Apply returned scoreDelta to the HUD
4. Poll the Hero coin count and synchronize coin/coin score
5. Update the camera
6. Start or advance the victory transition
7. If victory is not pending, start or advance the defeat transition
```

Victory is authoritative once it starts. A Hero death during the victory delay does
not replace the pending victory transition.

### Runtime update

`LevelRuntime::update()` performs:

```text
1. Intrinsic entity updates
   Hero -> Blocks -> Items -> Enemies -> Projectiles
2. WorldPhysicsSystem::update()
3. InteractionSystem::resolve()
4. Pit-death check for the Hero
5. Out-of-world projectile culling
6. GameWorld::removeInactiveEntities()
7. Return scoreDelta to PlayingState
```

An intrinsic entity update owns state changes, input, animation, timers, and its
own acceleration policy. World physics then integrates the resulting velocity and
resolves solid collisions.

## Physics and Collision Rules

### Division of responsibility

The current physics convention is intentionally split as follows:

| Concern | Owner |
|---|---|
| Hero acceleration and state-specific motion | Hero states |
| Item acceleration and movement policy | Concrete item classes |
| Projectile acceleration and phase timers | Concrete projectile classes |
| Enemy gravity (`1500`) | `WorldPhysicsSystem::resolveEnemies()` |
| Position integration against the world | `WorldPhysicsSystem` |
| Terrain and block collision response | `WorldPhysicsSystem` using `PhysicsEngine` |
| Entity-to-entity contact | `InteractionSystem` |

Enemy gravity is a deliberate temporary exception. Do not move it into Enemy states
unless the team explicitly starts a separate Enemy physics refactor.

Projectile gravity must not be added in `WorldPhysicsSystem`. A projectile updates
its own `velocity.y` in `Projectile::update()` using its gravity acceleration. The
physics system only integrates that velocity and resolves collisions.

### Hero collision

`resolveHero()`:

- skips a missing or dead Hero;
- resolves X before Y;
- collides with terrain and active solid blocks;
- calculates grounded state from top contacts;
- uses the Hero's previous and proposed bounds to detect an upward crossing;
- selects exactly one underside target independently of block vector order;
- chooses the nearest underside first, then the greatest horizontal overlap for
  adjacent blocks at the same height;
- adds a spawned block item to `GameWorld`.

`DeadState` owns the death movement. A dead Hero is not processed by world
collision, so the death animation can move through terrain and blocks.

### Item collision

Non-collidable items are integrated without solid collision. Collidable items:

- collide with terrain and active solid blocks;
- reverse through `Item::getCollision()` after a horizontal impact;
- receive grounded state from top contacts.

Gravity remains inside each concrete item implementation.

### Enemy collision

`resolveEnemies()` preserves the existing Enemy implementation:

- enemies below Y `800` are killed;
- `FlippingDeath` and `Squished` are skipped;
- gravity `1500` is applied in the system;
- terrain and active solid blocks participate in X and Y collision;
- a left/right collision flips direction once for that frame.

Do not add `Block::hit()` calls to Enemy collision. Enemies treat blocks only as
solid geometry.

### Projectile collision

Projectiles with `usesWorldPhysics() == true` collide with terrain and active solid
blocks. After collision resolution, the system calls
`Projectile::onSolidCollision()` so each concrete projectile decides whether to
bounce, die, splash, or change phase.

If collision changes `usesWorldPhysics()` or kills the projectile, the remaining
collision pass stops for that projectile.

### Invisible blocks

Before being hit, an `InvisibleBlock` is:

- not rendered;
- not solid for normal movement;
- hittable from below by an upward-moving Hero.

The upward swept test accepts any positive horizontal overlap, so a shallow edge
hit can reveal the block. Exact edge contact with zero overlap does not activate
it. A closer terrain ceiling wins over a block behind it.

After being hit, it becomes a visible, solid empty question block. Its hidden item
comes from the map object's `item` property. Current recognized values are:

- `coin` or an unknown/missing value -> Coin;
- `star` -> Star;
- `mushroom` or `flower` -> adaptive `PowerUpPrototype`.

## Interaction Order

`InteractionSystem::resolve()` uses this order:

```text
1. Hero <-> Item
2. Spinning Shell <-> Enemy
3. Hero <-> Enemy
4. Projectile <-> Target
5. Hero <-> LevelGoal
```

The system returns one `scoreDelta` for the frame. `PlayingState` applies it to the
HUD after `LevelRuntime::update()` returns.

### Dead-Hero guards

A dead Hero cannot:

- collect items;
- receive more Hero/Enemy interaction damage;
- be hit again by an enemy projectile;
- activate a level goal.

### Projectile factions

- Enemy projectiles test against the living Hero.
- Hero projectiles test against living, valid enemies.
- `FlippingDeath` and `Squished` enemies are not valid projectile targets.
- Score is awarded only when `onHitTarget()` reports that damage was applied.

Hero projectiles use two target passes because a projectile such as the Luigi water
bomb may expand its damage bounds on first impact. An `unordered_set` prevents the
same enemy from being resolved twice in one frame.

## Victory and Defeat Flow

### Victory

`InteractionSystem` activates the `LevelGoal`; `PlayingState` detects the activated
goal and owns the scene transition:

```text
Goal activated
 -> set Hero to FlyState
 -> wait 0.75 seconds while the world keeps updating
 -> change to VictoryState
```

### Defeat

`Hero::die()` is idempotent and enters `DeadState` only once. On the first frame
where `PlayingState` sees a dead Hero:

```text
Hero dead
 -> start a 2.0-second defeat delay
 -> keep updating the world and HUD timer
 -> DeadState continues its upward-and-falling effect without world collision
 -> change to GameOverState when the delay expires
```

The current system has no completed lives or respawn flow.

## Rendering Order

`LevelRuntime::renderWorld()` draws:

```text
Map -> Blocks -> Items -> Hero -> Enemies -> Projectiles
```

`PlayingState::render()` sets the camera view, renders the world, asks the HUD to
draw, and then restores the default view. `HUDManager::draw()` temporarily switches
the target to its default view and restores the camera view afterward, which keeps
HUD text screen-relative. If HUD elements move with the world, inspect both view
transitions first; broader HUD synchronization remains planned for a later phase.

## Where to Make a Change

| Change or bug | Primary location |
|---|---|
| Select map, tileset, or Hero type | `PlayingState` constructor |
| Parse a new Tiled object type | `LevelBuilder::build()` |
| Change map coordinate conversion | `LevelBuilder::build()` |
| Add/remove an owned entity | `GameWorld` |
| Add construction for a block/item/enemy | The relevant factory, then `LevelBuilder` |
| Change entity input, animation, timers, or acceleration | The entity or its state/form |
| Change terrain/block collision | `WorldPhysicsSystem` |
| Change Hero/item/enemy/projectile/goal interactions | `InteractionSystem` |
| Change score produced by gameplay contact | `InteractionSystem` or the contacted entity |
| Change HUD synchronization | `PlayingState` and `HUDManager` |
| Change camera behavior | `PlayingState` |
| Change victory/defeat delays or state transitions | `PlayingState` |
| Change frame update or render order | `LevelRuntime` |
| Change removal conditions | `GameWorld::removeInactiveEntities()` |

## Checklist for Adding a New Entity Category

When a feature does not fit an existing collection, verify all seven integration
points instead of placing everything in `PlayingState`:

1. Add ownership and accessors in `GameWorld`.
2. Construct it in `LevelBuilder` or through an in-game spawn callback.
3. Call its intrinsic `update()` in `LevelRuntime`.
4. Add world collision to `WorldPhysicsSystem` only if it needs solid collision.
5. Add entity interactions to `InteractionSystem`.
6. Render it in the intended order in `LevelRuntime::renderWorld()`.
7. Define a safe inactive/dead cleanup rule in `GameWorld`.

If it fits an existing category, prefer extending that category's polymorphic
interface instead of adding concrete-type checks to the systems.

## Debugging Guide

### Entity falls through terrain or a block

Check, in order:

1. whether the entity's intrinsic update produced the expected velocity;
2. whether its category is passed through `WorldPhysicsSystem`;
3. whether the block is active and `isSolid()`;
4. whether X resolution changed the hitbox position before Y resolution;
5. whether the final resolved hitbox position was copied back to the entity;
6. whether the map collider has the expected scaled position.

### Hero cannot hit a question or invisible block

Inspect `WorldPhysicsSystem::resolveHero()` and verify:

- the pre-resolution vertical velocity is negative;
- the previous Hero top is at or below the block bottom;
- the proposed Hero top crosses above the block bottom;
- Hero and block have positive horizontal overlap;
- `canBeHitFromBelow()` is true;
- no closer terrain underside preempts the block;
- among same-height blocks, the intended block has the greatest horizontal
  overlap with the Hero;
- the block has not already been hit;
- the returned item is added through `world.addItem()`.

### Enemy does not turn at a block

Inspect `WorldPhysicsSystem::resolveEnemies()`:

- the enemy must be alive and outside `FlippingDeath`/`Squished`;
- the block must be active and solid;
- the collision side must be `Left` or `Right`;
- `hitWall` should cause exactly one `flipDirection()` call per frame.

### Projectile moves but does not hit a target

Separate world collision from target interaction:

- trajectory/gravity: concrete projectile `update()`;
- terrain/block response: `WorldPhysicsSystem::resolveProjectiles()`;
- target selection and damage: `InteractionSystem::resolveProjectiles()`;
- Hero versus Enemy target selection: `ProjectileFaction`;
- score: the boolean returned by `Projectile::onHitTarget()`.

### Score or coin display is wrong

There are currently two paths:

- `InteractionSystem` returns score for enemy/projectile/goal interactions;
- `PlayingState` polls `Hero::getCoin()` and adds coin score separately.

Do not award the same score in both paths. Observer/event-based HUD synchronization
has not been implemented yet.

### Victory or defeat triggers unexpectedly

Set breakpoints at:

- `InteractionSystem::resolveHeroGoals()`;
- `Hero::die()`;
- the `m_victoryPending` and `m_defeatPending` branches in
  `PlayingState::update()`.

Remember that victory has priority and both transition delays intentionally keep
the world updating.

## Rules to Preserve During This Phase

- Keep `PlayingState` free of map parsing and entity-pair loops.
- Keep world ownership in `GameWorld`.
- Keep update, physics, interaction, and cleanup in their current order unless the
  behavioral effect of changing the order has been reviewed.
- Do not erase entities while iterating their collections.
- Do not apply projectile gravity in `WorldPhysicsSystem`.
- Keep Enemy gravity and state-name guards in `resolveEnemies()` until a dedicated
  Enemy refactor is approved.
- Keep terrain collision and entity interaction separate.
- Prefer capability methods such as `isSolid()`, `canBeHitFromBelow()`, and
  `usesWorldPhysics()` over concrete-type checks.
- Preserve dead-Hero guards in `InteractionSystem` and the pit-death guard in
  `LevelRuntime`.

## Current Temporary Decisions and Known Limitations

- The map, tileset, and Hero type are hardcoded in `PlayingState`.
- Three enemies are hardcoded at the end of `LevelBuilder::build()`.
- Enemy states are checked using strings in world physics and interactions.
- Enemy gravity is still owned by `WorldPhysicsSystem`.
- HUD values are updated by direct calls and coin polling, not Observer/events.
- Time-out behavior is deferred until the lives/session phase.
- Pit and projectile cleanup thresholds use fixed Y values.
- Map scale (`2`) and vertical offset (`272`) are hardcoded.
- Interaction loops are linear or nested scans; no spatial partitioning is used.

These limitations should be changed as separate, explicit tasks. Avoid quietly
mixing them into an unrelated gameplay fix because that makes regression debugging
much harder.

## Verification After a Gameplay Change

At minimum:

```powershell
cmake --build build --config Debug
git diff --check
```

Then manually verify the relevant update sequence, plus these shared regressions:

- Hero can stand on terrain and active blocks.
- Invisible blocks trigger only from below and become solid afterward.
- Items fall, land, and reverse at walls as expected.
- Enemies fall from ledges and collide with terrain and blocks.
- Hero and Enemy projectiles follow their expected trajectories and factions.
- A dead Hero falls through the world for two seconds without collecting or
  triggering anything.
- An activated goal reaches `VictoryState` and is not replaced by defeat.
- Inactive entities are removed only after the interaction phase.
