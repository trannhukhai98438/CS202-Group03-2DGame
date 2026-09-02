#pragma once
#include "Core/State.h"
#include <SFML/Graphics.hpp>
#include <array>
#include <memory>

class HUDManager;

class VictoryState : public State {
private:
	struct AnimatedLetter {
		sf::Text glyph;
		sf::Vector2f targetPosition;
		float delay{0.f};
		float initialRotation{0.f};
	};

	struct VictoryButton {
		sf::RectangleShape shape;
		sf::Text label;
	};

	void selectButton(int index);
	void activateButton(int index);
	void updateButtonAppearance();
	void updateLetterAnimation();
	void setStatusText(const sf::String& message);
	int buttonAt(sf::Vector2f point) const;

	sf::Font m_font;
	sf::RectangleShape m_fadeOverlay;
	std::array<AnimatedLetter, 8> m_letters;
	std::array<VictoryButton, 3> m_buttons;
	sf::Text m_statusText;
	std::shared_ptr<HUDManager> m_hudManager;
	sf::Vector2f m_launchPosition;
	float m_elapsedSeconds{0.f};
	bool m_buttonsVisible{false};
	bool m_initialHoverSynced{false};
	int m_selectedButton{0};
	int m_hoveredButton{-1};
	bool m_hasNextLevel{false};

public:
	explicit VictoryState(
		sf::Vector2f launchPosition = sf::Vector2f(640.f, 560.f),
		std::shared_ptr<HUDManager> hudManager = nullptr);
	void processEvents(sf::Event& event) override;
	void update(sf::Time dt) override;
	void render(sf::RenderWindow& window) override;
	bool rendersBelow() const override { return true; }
};
