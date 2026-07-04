# Weekly Progress Report

**Project:** Custom 2D Platformer - Group 3  
**Date / Week:** 29/6 to 4/7 | Week 04  

---

## 1. Weekly Overview
* **Primary Goal:** [Briefly describe the main objective for this week]
* **Completion Rate:** [Percentage]% of the weekly plan completed.

## 2. Individual Task Breakdown

**25125057 - Trần Minh Khoa** is responsible for the **Core Engine & Physics**.
* **Completed Tasks:**
   * Added a singleton Game class in `include/Core/Game.h` to ensure only one instance of the game can exist at a time, encapsulating the main window, game loop, and event handling logic.
   * Implemented the Game class methods in `src/Core/Game.cpp`, including the game loop (run), event processing, updating, and rendering, with a fixed timestep and window management using SFML.
   * Updated `src/main.cpp` to start the game by invoking `Game::getInstance().run()`.
   * Modified `CMakeLists.txt` to ensure SFML is built as a static library by setting `BUILD_SHARED_LIBS` to `OFF`.
* **Proof:** https://github.com/trannhukhai98438/CS20git add .2-Group03-2DGame/pull/1

**25125045 - Trần Như Khải** is in charge of the **Hero & Items Logic**.
* **Completed Tasks:** [Mô tả chi tiết các công việc đã làm được trong tuần]
* **Proof:** [Chèn link đến Commit, Pull Request trên GitHub, hoặc link ảnh chụp màn hình]

**25125024 - Đỗ Viết Hoàng Long** is handling the **Enemies & AI Design**.
* **Completed Tasks:** Designed the abstract base class `Enemy` using the **Template Method Pattern** to enforce a structured 3-step patrol AI (`checkObstacles`, `move`, `applyAnimation`). Created concrete header structures for `Goomba` and `Koopa`.
* **Note:** The current implementation relies on the base `Character` class. Since the official `Character.h` hasn't been pushed to the main repository yet, compilation might temporarily fail until the `Character` component is integrated.
* **Proof:** https://github.com/trannhukhai98438/CS202-Group03-2DGame/tree/feature/enemies-ai

**25125056 - Trần Đăng Khoa** is assigned to the development of the **Tilemap, HUD & Audio**.
* **Completed Tasks:** [Mô tả chi tiết các công việc đã làm được trong tuần]
* **Proof:** [Chèn link đến Commit, Pull Request trên GitHub, hoặc link ảnh chụp màn hình]

## 3. Challenges & Solutions

**Issue 1:** [Describe the problem encountered, e.g., Collision bugs, memory leaks, or Git merge conflicts]
**Solution 1:** [Describe how the team fixed or plans to fix it]

**Issue 2:** [Describe the problem encountered]
**Solution 2:** [Describe how the team fixed or plans to fix it]

## 4. Next Week's Action Plan
* **Core Mechanics:** [Tasks related to engine, physics, state management, or overall architecture]
* **Gameplay / Graphics:** [Tasks related to UI, level design, new entities, or AI behavior]
* **Expected Deadline:** [Target date for the next sprint's goals]
