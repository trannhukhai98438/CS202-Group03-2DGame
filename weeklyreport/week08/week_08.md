# Weekly Progress Report

**Project:** Custom 2D Platformer - Group 3  
**Date / Week:** 27/7 to 1/8 | Week 08  

---

## 1. Weekly Overview
* **Primary Goal:** [Briefly describe the main objective for this week]
* **Completion Rate:** [Percentage]% of the weekly plan completed.

## 2. Individual Task Breakdown

**25125057 - Trần Minh Khoa** is responsible for the **Core Engine & Physics**.
* **Completed Tasks:** [Mô tả chi tiết các công việc đã làm được trong tuần]
* **Proof:** [Chèn link đến Commit, Pull Request trên GitHub, hoặc link ảnh chụp màn hình]

**25125045 - Trần Như Khải** is in charge of the **Hero & Items Logic**.
* **Completed Tasks:** [Mô tả chi tiết các công việc đã làm được trong tuần]
* **Proof:** [Chèn link đến Commit, Pull Request trên GitHub, hoặc link ảnh chụp màn hình]

**25125024 - Đỗ Viết Hoàng Long** is handling the **Enemies & AI Design**.
* **Completed Tasks:**
  * Implemented the `Enemy` base class with patrol AI, sprite animation, and collision handling.
  * Implemented `Goomba` enemy with 4-frame walk animation, patrol behavior, and squish-on-stomp mechanic.
  * Implemented `Koopa` enemy with walk animation, Shell and SpinningShell states, and kick-on-side-collision mechanic.
* **Proof:**
  * https://github.com/trannhukhai98438/CS202-Group03-2DGame/commit/173e03b
  * https://github.com/trannhukhai98438/CS202-Group03-2DGame/commit/3f66765

**25125056 - Trần Đăng Khoa** is assigned to the development of the **Tilemap, HUD & Audio**.
* **Completed Tasks:**
  * Implemented HUDManager class.
  * Integrate HUDManager into PlayingState.
* **Proof:**
  * https://github.com/trannhukhai98438/CS202-Group03-2DGame/commit/47a2fe6432e2fb0192b544c57c608178cea11e00
  * https://github.com/trannhukhai98438/CS202-Group03-2DGame/commit/c9dd808b8cd6058e52755473695e7af4aa2db950

## 3. Challenges & Solutions

**Issue 1:** [Describe the problem encountered, e.g., Collision bugs, memory leaks, or Git merge conflicts]
**Solution 1:** [Describe how the team fixed or plans to fix it]

**Issue 2:** [Describe the problem encountered]
**Solution 2:** [Describe how the team fixed or plans to fix it]

## 4. AI Usage Notes
* Gemini. Gemini 3.1 Pro, Google, gemini.google.com, accessed 23:00 on July 24, 2026, prompt: "How do I implement HUDManager and integrate it into PlayingState in C++ SFML, and does decoupling player stats updates from the render pass optimize performance?" to finalize the in-game UI architecture, AI suggested the implementation blueprints for HUDManager and event-driven integration with PlayingState, Student reviewed the code and integrated the HUD system into the engine.

[Chat history]()

* Gemini. Gemini 3.1 Pro, Google, gemini.google.com, accessed 19:00 on July 29, 2026, prompt: "How to implement spritesheet animation and load custom texture shapes for Koopa and Goomba enemies in a C++ SFML platformer?" to research sprite sheet animation rendering and texture scaling, AI provided guidance on calculating source rectangles and dynamically adjusting sprite origins, Student reviewed the suggestions and implemented the sprite logic for Goomba and Koopa.

[Chat history]()

## 5. Next Week's Action Plan
* **Core Mechanics:** [Tasks related to engine, physics, state management, or overall architecture]
* **Gameplay / Graphics:** [Tasks related to UI, level design, new entities, or AI behavior]
* **Expected Deadline:** [Target date for the next sprint's goals]
