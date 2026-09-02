# List of Features

This document lists **40 implemented features** verified against the current source code. Each numbered entry describes a distinct capability that can be demonstrated in the game; debug-only shortcuts are intentionally excluded.

**Feature count:** 40  
## A. Game Flow, User Interface, and Persistence

1. **Main menu and New Game/Continue flow**

   The main hub provides Play, Character, Level, Guide, and Settings entries with both keyboard and mouse navigation. Play detects an existing save and presents Continue or New Game; an invalid save disables Continue safely.

   **Implementation:** `src/Core/MainMenuState.cpp`, `src/Core/ContinuePromptState.cpp`

2. **Three-character selection**

   The player can preview and select Mario, Luigi, or Flash through character cards, hover/selection feedback, mouse input, or A/D and arrow-key navigation. The selected hero is retained in the game context.

   **Implementation:** `src/Core/CharacterSelectState.cpp`, `src/Core/Game.cpp`, `src/Entities/Character/Hero/HeroFactory.cpp`

3. **Three-level selection and sequential progression**

   A centralized catalog exposes World 1-1, 1-2, and 1-3, maps each card to its TMJ file and HUD label, and determines the next level after a victory. A short intermission displays the selected world and remaining lives before play starts.

   **Implementation:** `include/Core/LevelCatalog.h`, `src/Core/LevelSelectState.cpp`, `src/Core/TransitionState.cpp`

4. **Multi-page in-game guide**

   A three-page guide explains controls, power-ups, playable heroes, special attacks, and cooldowns. Pages support Previous/Next navigation by mouse or keyboard.

   **Implementation:** `src/Core/GuideState.cpp`

5. **Live audio settings**

   Separate Theme Music and SFX sliders support mouse dragging and 5% keyboard steps from 0-100%, display percentage or MUTED, and preview the SFX level immediately.

   **Implementation:** `src/Core/SettingsState.cpp`, `src/Core/Game.cpp`

6. **Pause overlay and pause menu**

   Tab suspends gameplay and BGM while preserving the world beneath the overlay. Continue resumes play, Save reports success/failure, and Return to Menu exits the current session; all entries support mouse and keyboard control.

   **Implementation:** `src/Core/PausedState.cpp`, `src/Core/PlayingState.cpp`

7. **Complete save/load and quick-continue system**

   A save records the current map, HUD/timer, hero type/form/HP/position/coins/invincibility, enemy and boss state, altered blocks, remaining items, and live projectiles. Loading reconstructs the world and active presentation; JSON is validated and written through an atomic temporary-file replacement on platform-appropriate save paths.

   **Implementation:** `src/Managers/SaveManager.cpp`, `src/Core/PlayingState.cpp`, `src/Core/ContinuePromptState.cpp`

8. **Fixed HUD, scoring, coins, and extra lives**

   A camera-independent HUD displays zero-padded Score, Coins, World, Time, and Lives. Map coins and block coins award 100 points, enemy and goal rewards are accumulated, and every 100 coins grants one extra life plus 200 bonus points.

   **Implementation:** `src/Managers/HUDManager.cpp`, `src/Core/PlayingState.cpp`, `src/Entities/Item/Coin.cpp`

9. **Countdown and authoritative timeout**

   Every level starts with a countdown timer. Losing a life preserves the remaining time, while reaching zero always enters Game Over regardless of remaining lives; goal activation is resolved first so touching a goal freezes the timer immediately and cannot be overwritten by timeout.

   **Implementation:** `src/Managers/HUDManager.cpp`, `src/Core/PlayingState.cpp`

10. **Life loss, attempt rollback, retry, and Game Over**

   When the hero dies, score, coins, and lives are restored to their values at the start of the attempt, then exactly one life is deducted. The game plays a death transition and retries the same map if lives remain; otherwise it shows a selected-hero-specific Game Over screen and music.

   **Implementation:** `src/Core/PlayingState.cpp`, `src/Core/TransitionState.cpp`, `src/Core/GameOverState.cpp`

11. **Victory cinematic and post-level actions**

   Activating a goal places the hero in a stationary, damage-immune Cheer state, stops HUD counting, plays the goal's completion music, and animates the VICTORY text from the hero's screen position. The player can replay, return to menu, or advance to the next catalog level; Next Level is disabled after the final level.

   **Implementation:** `src/Core/PlayingState.cpp`, `src/Core/VictoryState.cpp`, `src/Entities/Character/Hero/HeroState/CheerState.cpp`

12. **Centralized contextual audio**

   WAV effects are scanned and cached, up to 16 SFX channels can overlap, and streamed MP3 BGM supports play, loop, pause, resume, and stop. Music changes with Overworld, Underground, Castle, Starman, death, victory, and Game Over contexts while BGM and SFX retain independent volume controls.

   **Implementation:** `src/Managers/SoundManager.cpp`, `src/Core/PlayingState.cpp`

## B. Hero Controls, Forms, Items, and Skills

13. **Momentum-based platform movement**

   Left/Right or A/D applies acceleration up to a capped run speed; releasing input applies friction. Jumping and walking off a ledge use different entry rules, with gravity, terminal fall speed, reduced air control, direction-aware sprites, and landing back into Idle or Run.

   **Implementation:** `src/Entities/Character/Hero/HeroState/IdleState.cpp`, `src/Entities/Character/Hero/HeroState/RunState.cpp`, `src/Entities/Character/Hero/HeroState/JumpState.cpp`, `include/Utilities/PhysicsConstants.h`

14. **Skid/slide on rapid reversal**

   Pressing the opposite direction while moving above the slide threshold enters a dedicated Slide state. Direction is locked, lower friction preserves the skid, and the hero returns to Idle only after losing momentum.

   **Implementation:** `src/Entities/Character/Hero/HeroState/RunState.cpp`, `src/Entities/Character/Hero/HeroState/SlideState.cpp`

15. **Crouching with a real hitbox change**

   Giant and Fire heroes can crouch with Down/S while grounded. The collision box changes from 32x64 to 32x48 while keeping the feet anchored, movement stops, and releasing the key restores the standing geometry.

   **Implementation:** `src/Entities/Character/Hero/HeroState/SitState.cpp`, `src/Entities/Character/Hero/HeroForm/GiantForm.cpp`, `src/Entities/Character/Hero/HeroForm/FireForm.cpp`

16. **Stomp combat and bounce feedback**

   A falling hero that reaches the upper portion of an enemy performs a stomp, invokes the enemy-specific stomp reaction, earns that enemy's score, and bounces upward. Side contact is dispatched separately to avoid applying damage twice.

   **Implementation:** `src/Entities/Character/Hero/Hero.cpp`, `src/Core/Gameplay/InteractionSystem.cpp`

17. **Small, Giant, and Fire/Special forms**

   All three heroes share a polymorphic form system. Small uses a 32x32 body, Giant and Fire/Special use 32x64, form changes select the appropriate texture/animation set, and every resize keeps the character's feet fixed in the world.

   **Implementation:** `src/Entities/Character/Hero/HeroForm/SmallForm.cpp`, `src/Entities/Character/Hero/HeroForm/GiantForm.cpp`, `src/Entities/Character/Hero/HeroForm/FireForm.cpp`

18. **Form-based damage, transformations, and invulnerability frames**

   Small dies from a hit. Giant and Fire/Special play a timed shrink sequence back to Small, lock movement during transformation, play damage feedback, and receive two seconds of blinking damage immunity to prevent repeated hits. Cheer also rejects damage during the victory handoff.

   **Implementation:** `src/Entities/Character/Hero/Hero.cpp`, `src/Entities/Character/Hero/HeroState/ShrinkState.cpp`, `src/Entities/Character/Hero/HeroForm/GiantForm.cpp`, `src/Entities/Character/Hero/HeroForm/FireForm.cpp`

19. **Adaptive Mushroom/Flower power-up chain**

   A block's PowerUp prototype creates a Mushroom for a Small hero and a Flower for an upgraded hero. Mushrooms rise from blocks, walk under gravity, reverse at walls, and animate Small-to-Giant growth; Flowers rise and promote Giant to each hero's Fire/Special form without downgrading an already upgraded hero.

   **Implementation:** `src/Entities/Item/PowerUpPrototype.cpp`, `src/Entities/Item/Mushroom.cpp`, `src/Entities/Item/Flower.cpp`, `src/Entities/Character/Hero/HeroState/GrowState.cpp`

20. **Bouncing Star and Starman mode**

   A Star emerges from a block, travels horizontally, reverses at walls, and continually bounces from the ground. Collection grants ten seconds of damage immunity, a cycling color effect, and temporary invincibility music before restoring the room's normal theme.

   **Implementation:** `src/Entities/Item/Star.cpp`, `src/Entities/Character/Hero/Hero.cpp`, `src/Core/PlayingState.cpp`

21. **Mario bouncing fireball**

   In Fire form Mario can launch a single-target fireball with a two-second cooldown. It travels under gravity, bounces from floors, expires after four seconds, and switches to a short explosion effect when it hits a wall, ceiling, or enemy.

   **Implementation:** `src/Entities/Character/Hero/Mario.cpp`, `src/Entities/Projectile/MarioFireball.cpp`

22. **Luigi water bomb and three-tile splash**

   In Special form Luigi throws a water bomb in an arc with a three-second cooldown. Contact with terrain or an enemy creates a 96-pixel-wide splash area, allowing one hit per enemy across roughly three tiles during its damage-resolution window.

   **Implementation:** `src/Entities/Character/Hero/Luigi.cpp`, `src/Entities/Projectile/LuigiWaterBomb.cpp`

23. **Flash thunder projectile and impact field**

   In Thunder form Flash fires a fast horizontal strike with a 2.5-second cooldown. It impacts on terrain, enemy contact, or lifetime expiry, expands from 64x24 to a 120x64 area, and plays a four-frame fading impact effect that can hit multiple targets once each.

   **Implementation:** `src/Entities/Character/Hero/Flash.cpp`, `src/Entities/Projectile/FlashThunder.cpp`, `src/Utilities/ThunderFlashTexture.cpp`

## C. Maps, Rooms, Physics, and Interactive Blocks

24. **Tiled TMJ map and multi-tileset rendering pipeline**

   Maps are loaded from Tiled JSON with dimensions, tile/object layers, offsets, visibility, TSX references, modern `class` or legacy `type`, and typed custom properties. Global tile IDs are resolved across tilesets and visible layers are batch-rendered in source order, including both terrain and object atlases.

   **Implementation:** `src/Managers/MapManager.cpp`, `src/Managers/LevelManager.cpp`, `include/Managers/MapData.hpp`

25. **Data-driven level construction and entity factories**

   `LevelBuilder` turns map objects into the selected hero, terrain colliders, interactive coins, blocks and their contained-item counts, lifters, enemies, bosses, and level goals. Dedicated factories and polymorphic ownership let map data select concrete types without hard-coding a particular map layout into `PlayingState`.

   **Implementation:** `src/Core/Gameplay/LevelBuilder.cpp`, `src/Core/Gameplay/GameWorld.cpp`, `src/Entities/Block/BlockFactory.cpp`, `src/Entities/Item/ItemFactory.cpp`, `src/Entities/Character/Enemy/EnemyFactory.cpp`

26. **Scrolling, room-aware camera**

   The camera follows the hero horizontally, clamps to world limits, snaps to physical pixels to avoid atlas seams, and vertically aligns the active room's bottom with the viewport so vertically stacked rooms do not leak onto screen.

   **Implementation:** `src/Core/PlayingState.cpp`, `src/Core/Gameplay/LevelRuntime.cpp`

27. **Playable-region switching and pit death**

   Named `playable_zone` rectangles define active rooms, their themes, and their lower death plane. Blocks and lifters are rendered for the active region, while heroes and enemies that fall below its valid play area enter their death/removal flow rather than relying on coordinates fixed for one map.

   **Implementation:** `src/Core/Gameplay/LevelRuntime.cpp`, `src/Core/Gameplay/GameWorld.cpp`

28. **Bidirectional, map-authored pipe travel**

   Named `pipe_in` and `pipe_out` objects are paired from map metadata. Travel validates the requested direction, grounding and alignment, uses an input latch/cooldown to prevent repeated teleports, preserves a form-independent feet anchor, and switches the active room after exit.

   **Implementation:** `src/Core/Gameplay/LevelRuntime.cpp`, `src/Core/PlayingState.cpp`

29. **Unified collision and interaction pipeline**

   AABB side classification, local terrain queries, per-axis resolution, swept hero ceiling checks, corner tie-breaking, and grounded detection support high-speed movement. The same world systems resolve items, enemies, shells, projectile factions/AOE target de-duplication, solid collisions, and goals.

   **Implementation:** `src/Core/PhysicsEngine.cpp`, `src/Core/Gameplay/WorldPhysicsSystem.cpp`, `src/Core/Gameplay/InteractionSystem.cpp`

30. **Overworld, Underground, and Castle palettes**

   Each playable region can select a normalized theme. One palette service supplies matching Brick and repeated Lifter textures for all three themes, while `PlayingState` synchronizes the room background and music whenever the active region changes.

   **Implementation:** `src/Core/Gameplay/BlockThemePalette.cpp`, `src/Core/Gameplay/LevelRuntime.cpp`, `src/Core/PlayingState.cpp`

31. **Breakable Brick blocks with debris**

   A Brick can release its configured hidden item first; otherwise Giant and Fire/Special heroes can destroy it from below. Destruction disables collision, records the changed tile, plays a break sound, and emits four gravity-driven debris pieces using the active theme texture.

   **Implementation:** `src/Entities/Block/BrickBlock.cpp`, `src/Core/Gameplay/GameWorld.cpp`

32. **Animated multi-item Question blocks**

   Question blocks animate while stocked, center each spawned item above the block, decrement a map-configured item count one hit at a time, and become an Empty block only after the final item is released. This supports multi-coin and other repeated-content blocks.

   **Implementation:** `src/Entities/Block/QuestionBlock.cpp`, `src/Entities/Block/Block.cpp`, `src/Core/Gameplay/LevelBuilder.cpp`

33. **Invisible surprise blocks**

   Invisible blocks begin hidden and non-solid to normal movement but can still detect an upward hit from the hero. Their first activation reveals a visible, solid Question-style block and releases the configured content.

   **Implementation:** `src/Entities/Block/InvisibleBlock.cpp`, `src/Core/Gameplay/WorldPhysicsSystem.cpp`

34. **Wrapping vertical Lifters**

   A map-authored `platform_up` or `platform_down` spawner is matched to a valid `platform_despawn` boundary while each Lifter owns its vertical velocity. Crossing one boundary teleports it to the opposite boundary, its collision surface can carry a rider, and its repeated themed texture supports platform widths other than a single fixed sprite size.

   **Implementation:** `src/Core/Gameplay/LevelBuilder.cpp`, `src/Entities/Block/Lifter.cpp`, `src/Core/Gameplay/WorldPhysicsSystem.cpp`

## D. Enemies, Boss, and Level Goals

35. **Goomba patrol and stomp defeat**

   Goombas patrol between bounds, reverse at obstacles, and use separate collision/animation components. A stomp changes them to a squished state, awards score, then fades and removes them; a spinning shell instead knocks them into the flipping-death presentation.

   **Implementation:** `src/Entities/Character/Enemy/Goomba.cpp`, `src/Entities/Character/Enemy/GoombaPhysics.cpp`, `src/Entities/Character/Enemy/EnemyState.cpp`

36. **Koopa shell lifecycle and shell-chain combat**

   Stomping a Koopa creates a stationary shell; another stomp or side contact kicks it away from the hero. A spinning shell damages enemies for combo score, can be stomped back to rest, protects the kicker with a short safety cooldown, and wakes back into a Koopa after remaining idle.

   **Implementation:** `src/Entities/Character/Enemy/Koopa.cpp`, `src/Entities/Character/Enemy/KoopaPhysics.cpp`, `src/Core/Gameplay/InteractionSystem.cpp`

37. **Witch ranged attack and persistent potion puddle**

   Witches patrol and periodically enter a throw state synchronized to their animation. Their arcing enemy projectile uses gravity, damages the hero on contact, and shatters against terrain into a 96x18 puddle hazard that remains for 1.5 seconds.

   **Implementation:** `src/Entities/Character/Enemy/Witch.cpp`, `src/Entities/Character/Enemy/WitchPhysics.cpp`, `src/Entities/Character/Enemy/Potion.cpp`

38. **Three-phase ThorKing boss encounter**

   ThorKing progresses through three phases with rising patrol/roll speed, shorter stuns, fire bursts, crouch-to-roll-to-wall-hit-to-stun cycles, roar transitions, and phase-three straight/sky-drop meteors. The arena boundary remains locked while the boss lives, and phase three adds darkness, lightning damage, craters, fissures, and debris effects.

   **Implementation:** `src/Entities/Character/Enemy/ThorKing.cpp`, `src/Entities/Character/Enemy/ThorKingState.cpp`, `src/Entities/Character/Enemy/ThorKingPhysics.cpp`, `src/Core/PlayingState.cpp`, `src/Core/Gameplay/LevelRuntime.cpp`

39. **Height-based Flag goal**

   The Flag is a one-shot goal trigger whose reward scales from 100 to 1,000 points according to how high the hero touches the pole. Activation produces the flagpole sound and selects the course-clear music for the victory flow.

   **Implementation:** `src/Entities/Goal/Flag.cpp`, `src/Core/Gameplay/InteractionSystem.cpp`

40. **Princess rescue goal**

   A map can mark its end trigger as a Princess goal. Touching the Princess activates the goal only once, awards 500 points, and selects the ending theme before entering the shared Cheer and Victory sequence.

   **Implementation:** `src/Entities/Goal/Princess.cpp`, `src/Core/Gameplay/LevelBuilder.cpp`, `src/Core/Gameplay/InteractionSystem.cpp`

## Coverage Notes

- The list covers the current user-facing flow, HUD/economy, persistence, hero mechanics, every item type, every playable hero skill, the generalized map/runtime systems, all interactive block types, the Lifter, every enemy family, the boss encounter, and both goal types.
- Internal debug hotkeys and empty/no-op methods are not counted as features.
- Architecture is mentioned only where it directly enables an observable capability, such as loading different maps, switching room themes, restoring a complete save, or dispatching distinct entity behavior.
