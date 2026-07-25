#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class LevelManager : public sf::Drawable, public sf::Transformable {
private:
    sf::VertexArray m_vertices;
    sf::Texture m_tileset;
    float m_tileSize;
    int m_width;
    int m_height;
    std::vector<std::vector<int>> m_grid;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
    LevelManager();
    ~LevelManager() = default;

    bool loadLevel(const std::string& levelPath, const std::string& tilesetPath, float tileSize = 32.0f);

    const std::vector<std::vector<int>>& getGrid() const { return m_grid; }
    float getTileSize() const { return m_tileSize; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
};