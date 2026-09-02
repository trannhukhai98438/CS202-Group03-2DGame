# Weekly Progress Report

**Project:** Custom 2D Platformer - Group 3  
**Date / Week:** 3/8 to 8/8 | Week 09  

---

## 1. Weekly Overview
* **Primary Goal:**
  * Merge all individual feature branches into the primary `dev` branch to consolidate the codebase.
  * Conduct system-wide adjustments, code reviews, and bug squashing to ensure stability across the integrated architecture.
  * Finalize the core implementations for the Hero class, interactive Bricks, classic Items, and Enemy entities (Goombas, Koopas).
  * Construct a functional demo map layout to actively playtest all integrated features, verify complex entity interactions, and identify runtime bugs.
* **Completion Rate:** 70% of the weekly plan completed.

## 2. Individual Task Breakdown

**25125057 - Trần Minh Khoa** is responsible for the **Core Engine & Physics**.
* **Completed Tasks:**
* Adjust camera view- that follows hero
* **Proof:** https://github.com/trannhukhai98438/CS202-Group03-2DGame/tree/Demo

**25125045 - Trần Như Khải** is in charge of the **Hero & Items Logic**.
* **Completed Tasks:**
* Completed implementation for bricks, items
* Adjusted hero, bricks, items logic with Physical Engine
* Added temporary logic interactions between entities for demo
* **Proof:** https://github.com/trannhukhai98438/CS202-Group03-2DGame/tree/Demo

**25125024 - Đỗ Viết Hoàng Long** is handling the **Enemies & AI Design**.
* **Completed Tasks:** 
* Implemented new Enemy: Witch with projective logic
* Restructed files, which following strictly SOLID principle
* **Proof:** https://github.com/trannhukhai98438/CS202-Group03-2DGame/tree/Demo

**25125056 - Trần Đăng Khoa** is assigned to the development of the **Tilemap, HUD & Audio**.
* **Completed Tasks:**
  * Add MapManager to load maps for the level.
  * Improving LevelManager and MapManager.
* **Proof:**
  * https://github.com/trannhukhai98438/CS202-Group03-2DGame/commit/d0010379175908b86f8f238e63077125106fa580
  * https://github.com/trannhukhai98438/CS202-Group03-2DGame/commit/d52d2d47a3c5e52c09a2cadc957e261617d95a80

## 3. Challenges & Solutions

**Issue 1:** The fact that **Hero** and **Enemy** using the same interface **Character** has no advantages in design structures. It makes derived classes depend strongly on it
**Solution 1:** Eliminated interface class **Character**, logic of **Character** class was transfered into **Hero** and **Enemy** class


## 4. AI Usage Notes
* Gemini. Gemini 3.1 Pro, Google, gemini.google.com, accessed 23:00 on Aug 4, 2026, prompt: "How do I implement MapManager and integrate it into PlayingState in C++ SFML, and does using vertex arrays with viewport culling optimize tilemap rendering performance?" to finalize the game world map architecture, AI suggested the implementation blueprints for MapManager and tile rendering optimization strategies, Student reviewed the code and integrated the map system into the engine.


## 5. Next Week's Action Plan
* **Core Mechanics**: Integrate LevelManagement logic into the main project architecture, finalize the OOP design logic for core classes, and engineer a robust system to handle cross-entity interactions (collisions, state triggers).

* **Gameplay / Graphics**: Upgrade the overall user interface (UI) and seamlessly integrate the HUD and tilemap systems into the rendering loop.
* **Expected Deadline:** 15/8
