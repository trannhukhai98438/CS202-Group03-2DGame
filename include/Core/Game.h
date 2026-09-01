#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>
#include "Core/State.h"
#include "Entities/Character/Hero/HeroFactory.h"

class Game {
private:
	enum class PendingStateAction {
		None,
		Push,
		Pop,
		Replace,
		ClearAndPush
	};

	Game();
	sf::RenderWindow m_window;

	const sf::Time TimePerFrame = sf::seconds(1.f / 60.f);
	std::vector<std::unique_ptr<State>> m_states; // State stack
	PendingStateAction m_pendingStateAction{PendingStateAction::None};
	std::unique_ptr<State> m_pendingState;
	HeroType m_selectedHero{HeroType::Mario};
	std::string m_selectedLevelPath{"assets/maps/levels/1-1.tmj"};
	std::string m_selectedWorldName{"WORLD 1-1"};
	float m_themeMusicVolume{80.0f};
	float m_sfxVolume{100.0f};
	bool m_tabKeyDown{false};
	bool m_enterKeyDown{false};
	void applyPendingStateAction();
	void processEvents();
	void update(sf::Time dt);
	void render();

public:
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;

	static Game& getInstance() {
		static Game instance;
		return instance;
	}

	void run();
	void pushState(std::unique_ptr<State> state);
	void popState();
	void changeState(std::unique_ptr<State> state);
	void clearStatesAndChange(std::unique_ptr<State> state);
	void setSelectedHero(HeroType heroType) { m_selectedHero = heroType; }
	HeroType getSelectedHero() const { return m_selectedHero; }
	void setSelectedLevel(std::string levelPath, std::string worldName);
	const std::string& getSelectedLevelPath() const {
		return m_selectedLevelPath;
	}
	const std::string& getSelectedWorldName() const {
		return m_selectedWorldName;
	}
	void setThemeMusicVolume(float volume);
	float getThemeMusicVolume() const { return m_themeMusicVolume; }
	void setSfxVolume(float volume);
	float getSfxVolume() const { return m_sfxVolume; }
};
