#include "Managers/LevelManager.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

LevelManager::LevelManager() 
    : m_tileSize(32.0f), m_width(0), m_height(0) {
}

bool LevelManager::loadLevel(const std::string& levelPath, const std::string& tilesetPath, float tileSize) {
    m_tileSize = tileSize;
    m_grid.clear();

    // 1. Load the Texture image containing the brick frame.
    if (!m_tileset.loadFromFile(tilesetPath)) {
        std::cerr << "[LevelManager] ERROR: cannot load tileset from" << tilesetPath << std::endl;
        return false;
    }

    // 2. Open and read the TXT file containing the map matrix.
    std::ifstream file(levelPath);
    if (!file.is_open()) {
        std::cerr << "[LevelManager] ERROR: Cannot open map file from " << levelPath << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::vector<int> row;
        std::stringstream ss(line);
        int tileType;
        
        while (ss >> tileType) {
            row.push_back(tileType);
        }
        m_grid.push_back(row);
    }
    file.close();

    m_height = static_cast<int>(m_grid.size());
    m_width = m_height > 0 ? static_cast<int>(m_grid[0].size()) : 0;

    // 3. Initialize the SFML (VertexArray) array.
    m_vertices.setPrimitiveType(sf::Quads);
    m_vertices.resize(m_width * m_height * 4);

    int tilesetColumns = m_tileset.getSize().x / static_cast<int>(m_tileSize);

    for (int i = 0; i < m_height; ++i) {
        for (int j = 0; j < m_width; ++j) {
            int tileNumber = m_grid[i][j];

            if (tileNumber == 0) continue;

            // Determine the corresponding position of the tile in the tileset image file.
            int tu = (tileNumber - 1) % tilesetColumns;
            int tv = (tileNumber - 1) / tilesetColumns;

            // Move the cursor to the four vertices of the current cell.
            sf::Vertex* quad = &m_vertices[(i * m_width + j) * 4];

            // Set the display coordinates on the screen.
            quad[0].position = sf::Vector2f(j * m_tileSize, i * m_tileSize);
            quad[1].position = sf::Vector2f((j + 1) * m_tileSize, i * m_tileSize);
            quad[2].position = sf::Vector2f((j + 1) * m_tileSize, (i + 1) * m_tileSize);
            quad[3].position = sf::Vector2f(j * m_tileSize, (i + 1) * m_tileSize);

            // Crop the corresponding image from the tileset file.
            quad[0].texCoords = sf::Vector2f(tu * m_tileSize, tv * m_tileSize);
            quad[1].texCoords = sf::Vector2f((tu + 1) * m_tileSize, tv * m_tileSize);
            quad[2].texCoords = sf::Vector2f((tu + 1) * m_tileSize, (tv + 1) * m_tileSize);
            quad[3].texCoords = sf::Vector2f(tu * m_tileSize, (tv + 1) * m_tileSize);
        }
    }

    return true;
}

void LevelManager::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    states.texture = &m_tileset;
    target.draw(m_vertices, states);
}