# Weekly Progress Report

**Project:** Custom 2D Platformer - Group 3  
**Date / Week:** 12/7 to 18/7 | Week 06  

---

## 1. Weekly Overview
* **Primary Goal:**
  * Implement basic gravity/collision and integrate complex collision detection (e.g., Mario stomping on Goombas).
  * Finish Mario's size-changing logic.
  * Develop the tilemap renderer to display a static Mario level (bricks, pipes) from a text file.
  * Spawn classic items (Mushroom, Coin).
  * Spawn Goombas/Koopas and implement basic walking/bouncing behavior.
  * Design and implement core game state manager (Main Menu, Pause, Play)
* **Completion Rate:** 25% of the weekly plan completed.

## 2. Individual Task Breakdown

**25125057 - Trần Minh Khoa** is responsible for the **Core Engine & Physics**.
* **Completed Tasks:**
  * Engine Architecture: Designed and implemented the core game state manager using the State Design Pattern.
  * State Interface: Created a pure abstract State base class (State.h) to enforce a strict contract for all game screens   (event processing, updating, and rendering).
  * State Management: Integrated a std::stack into the Game singleton to handle seamless screen transitions via pushState, popState, and changeState methods.
  * Concrete States: Successfully implemented MainMenuState, PlayingState (core gameplay placeholder), and PausedState, completely wiring up the navigation flow between them (e.g., using 'Enter' to start, 'ESC' to pause/resume).
  * UI & Resolution: Configured a strict 1280x720 (16:9) window resolution to prevent game coordinate distortion, loaded custom .ttf fonts (SuperMario256.ttf), and applied dynamic mathematical bounds (getLocalBounds()) to ensure UI text remains perfectly centered.
* **Proof:** https://github.com/trannhukhai98438/CS202-Group03-2DGame/commit/7a7de1094d17c8f03e2c3443c80168a056e39fcd

**25125045 - Trần Như Khải** is in charge of the **Hero & Items Logic**.
* **Completed Tasks:** no tasks completed this week

**25125024 - Đỗ Viết Hoàng Long** is handling the **Enemies & AI Design**.
* **Completed Tasks:** no tasks completed this week

**25125056 - Trần Đăng Khoa** is assigned to the development of the **Tilemap, HUD & Audio**.
* **Completed Tasks:** no tasks completed this week

## 3. Challenges & Solutions

no challenges or solutions in this week.

## 4. AI Usage Notes
* Gemini. Gemini 3.1 Pro, Google, gemini.google.com, accessed 18:30 on July 17, 2026, prompt: "How should I design the abstract State class and integrate it with a stack in my Game engine to handle seamless transitions between Menu, Playing, and Paused screens using C++?" to architect the core game state manager, AI suggested an Abstract Base Class and a std::stack implementation for state transitions, Student reviewed it and accepted the architecture.
* Gemini. Gemini 3.1 Pro, Google, gemini.google.com, accessed 19:00 on July 17, 2026, prompt: "When rendering SFML text using a custom .ttf font, how can I mathematically lock the UI elements to the exact center of the screen, and how should I handle window resizing to prevent the text and game coordinates from stretching?" to resolve UI distortion during window scaling, AI suggested using getLocalBounds() for dynamic centering and locking the window to a strict 1280x720 resolution, Student reviewed it and accepted the static resolution approach.
## 5. Next Week's Action Plan
* **Core Mechanics:** 
  * Prioritize and complete the delayed physics engine: implement basic gravity and precise AABB collision detection[cite: 3, 5].
  * Integrate complex entity collision logic (e.g., Mario stomping on Goombas)[cite: 3, 5].
  * Finalize Mario's mechanics, including size-changing logic (Strategy Pattern) and state transitions (invincibility frames)[cite: 3, 5].
* **Gameplay / Graphics:** 
  * Fully develop the tilemap renderer to load and display static Mario levels (bricks, pipes) from text/csv files.
  * Spawn classic items (Mushroom, Coin) using the Factory Pattern.
  * Spawn classic enemies (Goombas, Koopas) and implement their basic walking/bouncing patrol AI.
  * Implement the HUD (score, coins, lives) via the Observer Pattern.
  * Begin Level Design by constructing the first classic level layout and placing entities strategically.
* **Expected Deadline:** 25/7/2026