#pragma once

#include <array>
#include <string_view>

namespace LevelCatalog {
struct LevelDefinition {
    const char* number;
    const char* name;
    const char* status;
    const char* mapPath;
    const char* worldName;
    const char* hudWorldName;
};

inline constexpr std::array<LevelDefinition, 3> LEVELS = {{
    {"1", "WORLD 1-1", "READY", "assets/maps/levels/1-1.tmj",
     "WORLD 1-1", "1-1"},
    {"2", "WORLD 1-2", "READY", "assets/maps/levels/1-2.tmj",
     "WORLD 1-2", "1-2"},
    {"3", "WORLD 1-3", "READY",
     "assets/maps/levels/1-3.tmj", "WORLD 1-3", "1-3"}
}};

inline const LevelDefinition* findByMapPath(std::string_view mapPath) {
    for (const LevelDefinition& level : LEVELS) {
        if (mapPath == level.mapPath) return &level;
    }
    return nullptr;
}

inline const LevelDefinition* nextAfter(std::string_view mapPath) {
    for (std::size_t i = 0; i + 1 < LEVELS.size(); ++i) {
        if (mapPath == LEVELS[i].mapPath) return &LEVELS[i + 1];
    }
    return nullptr;
}
}
