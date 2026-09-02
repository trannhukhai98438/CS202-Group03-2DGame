# Mario Game Project

A custom two-dimensional platform game developed for the CS202 course by Group 03 at the University of Science, Vietnam National University Ho Chi Minh City.

## 1. Team and Supervision

- **Institution:** University of Science, VNU-HCM
- **Faculty:** Faculty of Information Technology
- **Course:** CS202 - Programming Systems
- **Group:** Group 03
- **Instructor / Supervisor:**
  - PhD. DINH Ba Tien
  - MSc. TRUONG Phuoc Loc
  - MSc. HO Tuan Thanh

### Team Members

1. **Tran Minh Khoa (25125057):** Core engine, game loop, game states, physics, and collision handling.

2. **Tran Nhu Khai (25125045):** Team lead, project workflow, hero mechanics, character hierarchy, items, and power-ups.

3. **Do Viet Hoang Long (25125024):** Enemy hierarchy, enemy artificial intelligence, patrol behavior, and combat interactions.

4. **Tran Dang Khoa (25125056):** Level system, map loading, HUD, UI/UX, and audio management.

All members also contributed to integration, debugging, testing, and gameplay refinement.

## 2. Project Overview

Mario Game Project is a side-scrolling 2D platformer written in **C++17** with **SFML 2.6.1**. It is inspired by classic Mario gameplay while using a custom object-oriented architecture.

The player can choose between **Mario, Luigi, and Flash**, each with distinct visuals and projectile-based special abilities. The game currently contains three levels: **World 1-1, World 1-2, and World 1-3**. Levels are loaded from Tiled map files and can define terrain, interactive blocks, items, enemies, moving platforms, pipes, playable regions, themes, and level goals.

Main gameplay systems include:

- Character movement, jumping, sliding, crouching, damage, transformations, and victory/death states.
- Small, Giant, and Fire/Special hero forms.
- Coins, Mushrooms, Flowers, Stars, and multi-item blocks.
- Mario fireballs, Luigi water bombs, and Flash thunder attacks.
- Goomba, Koopa, Witch, and the multi-phase ThorKing boss.
- Breakable Bricks, Question blocks, Invisible blocks, and wrapping Lifters.
- Overworld, Underground, and Castle visual/audio themes.
- HUD, timer, lives, score, pause, victory, Game Over, save, and load systems.
- Keyboard and mouse navigation for menus, character selection, level selection, guide, and settings.

The architecture applies object-oriented principles and patterns such as State, Factory, Prototype, Strategy, Singleton, and Observer-style callbacks. A more detailed feature inventory is available in `ListOfFeatures.md`.

## 3. Project Structure

The following tree shows the main repository structure. Generated build directories are not included.

```text
CS202-Group03-2DGame/
|-- CMakeLists.txt                 # Root CMake entry point
|-- README.md                      # Project introduction and build guide
|-- ListOfFeatures.md              # Detailed implemented feature list
|-- .gitignore
|-- .vscode/                       # Local VS Code configuration
|-- AI_Usage_Declaration/          # AI usage declaration in Markdown/PDF
|-- Contribution/                  # Team contribution and evaluation material
|-- Project_Report/                # LaTeX, Markdown, diagrams, images, and PDF report
`-- Project-2DGame/                # Game source package
    |-- CMakeLists.txt             # Reusable game target configuration
    |-- assets/
    |   |-- audio/
    |   |   |-- bgm/               # Background music
    |   |   `-- sfx/               # Sound effects
    |   |-- fonts/                 # HUD and menu fonts
    |   |-- maps/
    |   |   |-- levels/            # Runtime Tiled JSON/TMJ levels
    |   |   |-- resources/         # Tilesets and map images
    |   |   `-- tilemaps/          # Editable Tiled TMX maps
    |   `-- textures/              # Hero, item, block, enemy, UI, and VFX sprites
    |-- include/
    |   |-- Core/                  # Game states, physics interfaces, gameplay facade
    |   |   `-- Gameplay/          # World, builder, runtime, physics, interactions
    |   |-- Entities/
    |   |   |-- Block/             # Brick, Question, Invisible block, and Lifter
    |   |   |-- Character/         # Hero and enemy class hierarchies
    |   |   |-- Goal/              # Flag and Princess goals
    |   |   |-- Item/              # Collectible and power-up interfaces
    |   |   `-- Projectile/        # Hero projectile interfaces
    |   |-- Managers/              # Map, level, HUD, save, and sound managers
    |   |-- Utilities/             # Animation and shared physics utilities
    |   `-- nlohmann/              # Vendored JSON headers
    `-- src/
        |-- main.cpp               # Application entry point
        |-- Core/                  # Game state and gameplay implementations
        |-- Entities/              # Block, character, item, goal, and projectile code
        |-- Managers/              # Resource and persistence implementations
        `-- Utilities/             # Animation and shared utility implementations
```

## 4. Building the Project

### Requirements

- CMake 3.14 or newer.
- A C++17-compatible compiler.
- Git and an Internet connection for the first configuration, because CMake fetches SFML 2.6.1 automatically.
- On Windows, a 64-bit MinGW-w64 toolchain is recommended. The current configuration has been verified with MinGW Makefiles.

### Build from the Repository Root

This is the recommended workflow for development and CI:

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 4
```

The Windows executable will be generated at:

```text
build/Project-2DGame/Custom2DPlatformer.exe
```

### Build Directly from the Game Directory

The child CMake project can also be configured independently:

```powershell
cmake -S Project-2DGame -B build-game -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build-game --parallel 4
```

The Windows executable will then be generated at:

```text
build-game/Custom2DPlatformer.exe
```

Use a different build directory for each configuration mode. Do not reuse the same CMake cache for both the repository root and `Project-2DGame` as the source directory.

### Running

CMake copies the complete `assets` directory and, on Windows, `openal32.dll` beside the executable. Run the game with its output directory as the working directory because runtime resources use relative `assets/...` paths.

Example for a root build:

```powershell
Set-Location build/Project-2DGame
./Custom2DPlatformer.exe
```

## 5. Links and Related Notes

### Project Resources

- **Source code:** [CS202 Group 03 - 2D Game on GitHub](https://github.com/trannhukhai98438/CS202-Group03-2DGame)
- **Team contribution record:** [Group 03 contribution spreadsheet](https://docs.google.com/spreadsheets/d/16kN_1ETpQ_CZTqht7uKMTEQpaRrnlutVuxIZDjjk520/edit?gid=1781385760#gid=1781385760)
- **Video demonstration:** [Mario Game Project demo on YouTube](https://youtu.be/3uOq83P2HQk)

### Project Documents

- **Feature list:** [ListOfFeatures.md](ListOfFeatures.md)
- **Project report:** [PDF version](Project_Report/main.pdf) and [Markdown version](Project_Report/markdownReport/main.md)
- **AI usage declaration:** [AI_USAGE_DECLARATION.md](AI_Usage_Declaration/AI_USAGE_DECLARATION.md)
- **Contribution evaluation:** [Group03-ProjectEvaluation.xlsx](Contribution/Group03-ProjectEvaluation.xlsx)

### Notes

- The project report was originally authored in LaTeX. Its Markdown version was generated to satisfy the additional submission-format requirements.
- The Google Sheets contribution record may require the appropriate sharing permission to access.
- This is an academic, non-commercial project inspired by the Super Mario series. Third-party names, characters, music, sound effects, and visual assets remain the property of their respective owners.
