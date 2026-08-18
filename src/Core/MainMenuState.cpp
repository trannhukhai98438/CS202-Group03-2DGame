#include "Core/MainMenuState.h"
#include "Core/Game.h"
#include "Core/PlayingState.h"
#include "Core/TransitionState.h"
#include "Core/CharacterSelectState.h"
#include "Core/GuideState.h"
#include <array>
#include <iostream>

MainMenuState::MainMenuState() {
	if (!m_font.loadFromFile("assets/fonts/SuperMario256.ttf")) {
		std::cerr << "Error loading font\n";
	}
	if (!m_backgroundTexture.loadFromFile("assets/textures/menuBackground.png")) {
		std::cerr << "Error loading menu background\n";
	}
	m_backgroundSprite.setTexture(m_backgroundTexture);
	const sf::Vector2u textureSize = m_backgroundTexture.getSize();
	if (textureSize.x > 0 && textureSize.y > 0) {
		m_backgroundSprite.setScale(1280.f / textureSize.x,
		                            720.f / textureSize.y);
	}

	m_overlay.setSize({1280.f, 720.f});
	m_overlay.setFillColor(sf::Color(0, 0, 0, 70));

	m_titleText.setFont(m_font);
	m_titleText.setString("SUPER MARIO");
	m_titleText.setCharacterSize(72);
	m_titleText.setFillColor(sf::Color(255, 220, 55));
	m_titleText.setOutlineColor(sf::Color(170, 30, 25));
	m_titleText.setOutlineThickness(5.f);
	const sf::FloatRect titleBounds = m_titleText.getLocalBounds();
	m_titleText.setOrigin(titleBounds.left + titleBounds.width / 2.f,
	                      titleBounds.top + titleBounds.height / 2.f);
	m_titleText.setPosition(640.f, 135.f);

	const std::array<std::string, 5> labels = {
		"PLAY", "CHARACTER", "LEVEL", "GUIDE", "SETTINGS"
	};
	for (std::size_t i = 0; i < m_buttons.size(); ++i) {
		auto& button = m_buttons[i];
		button.shape.setSize({330.f, 54.f});
		button.shape.setOrigin(165.f, 27.f);
		button.shape.setPosition(640.f, 245.f + static_cast<float>(i) * 68.f);
		button.shape.setOutlineThickness(3.f);

		button.label.setFont(m_font);
		button.label.setString(labels[i]);
		button.label.setCharacterSize(24);
		const sf::FloatRect bounds = button.label.getLocalBounds();
		button.label.setOrigin(bounds.left + bounds.width / 2.f,
		                       bounds.top + bounds.height / 2.f);
		button.label.setPosition(button.shape.getPosition());
	}
	updateButtonAppearance();

	m_statusText.setFont(m_font);
	m_statusText.setCharacterSize(18);
	m_statusText.setFillColor(sf::Color::White);
	m_statusText.setPosition(640.f, 605.f);
}

void MainMenuState::processEvents(sf::Event& event) {
	if (event.type == sf::Event::MouseMoved) {
		m_hoveredButton = buttonAt({static_cast<float>(event.mouseMove.x),
		                            static_cast<float>(event.mouseMove.y)});
		updateButtonAppearance();
	}

	if (event.type == sf::Event::MouseButtonPressed
		&& event.mouseButton.button == sf::Mouse::Left) {
		const int clicked = buttonAt({static_cast<float>(event.mouseButton.x),
		                              static_cast<float>(event.mouseButton.y)});
		if (clicked == 0) {
			Game::getInstance().startNewGame();
			Game::getInstance().changeState(std::make_unique<TransitionState>());
			return;
		}
		if (clicked == 1) {
			Game::getInstance().changeState(std::make_unique<CharacterSelectState>());
			return;
		}
		if (clicked == 3) {
			Game::getInstance().changeState(std::make_unique<GuideState>());
			return;
		}
		if (clicked == 2 || clicked == 4) {
			m_statusText.setString(clicked == 2
				? "LEVELS - COMING SOON"
				: "SETTINGS - COMING SOON");
			const sf::FloatRect bounds = m_statusText.getLocalBounds();
			m_statusText.setOrigin(bounds.left + bounds.width / 2.f,
			                       bounds.top + bounds.height / 2.f);
		}
	}

	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
		Game::getInstance().startNewGame();
		Game::getInstance().changeState(std::make_unique<TransitionState>());
	}
}

void MainMenuState::update(sf::Time dt) {
	// For now: empty because menu has no moving parts.
}

void MainMenuState::render(sf::RenderWindow& window) {
	window.draw(m_backgroundSprite);
	window.draw(m_overlay);
	window.draw(m_titleText);
	for (const auto& button : m_buttons) {
		window.draw(button.shape);
		window.draw(button.label);
	}
	window.draw(m_statusText);
}

int MainMenuState::buttonAt(sf::Vector2f point) const {
	for (std::size_t i = 0; i < m_buttons.size(); ++i) {
		if (m_buttons[i].shape.getGlobalBounds().contains(point)) {
			return static_cast<int>(i);
		}
	}
	return -1;
}

void MainMenuState::updateButtonAppearance() {
	for (std::size_t i = 0; i < m_buttons.size(); ++i) {
		const bool hovered = static_cast<int>(i) == m_hoveredButton;
		auto& button = m_buttons[i];
		button.shape.setFillColor(hovered
			? sf::Color(245, 195, 45, 235)
			: sf::Color(20, 35, 65, 210));
		button.shape.setOutlineColor(hovered
			? sf::Color::White
			: sf::Color(230, 230, 230, 220));
		button.label.setFillColor(hovered
			? sf::Color(125, 25, 20)
			: sf::Color::White);
	}
}
