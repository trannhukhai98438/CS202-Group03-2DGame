#include "Core/PausedState.h"
#include "Core/Game.h"
#include "Core/MainMenuState.h"

#include <array>
#include <iostream>
#include <memory>

PausedState::PausedState() {
    if (!m_font.loadFromFile("assets/fonts/SuperMario256.ttf")) {
        std::cerr << "ERROR: Failed to load font!\n";
    }

	m_fadeOverlay.setSize({1280.f, 720.f});
	m_fadeOverlay.setFillColor(sf::Color(0, 0, 0, 160));

    m_pauseText.setFont(m_font);
    m_pauseText.setString("PAUSED");
    m_pauseText.setCharacterSize(60);
	m_pauseText.setFillColor(sf::Color(255, 220, 55));
	m_pauseText.setOutlineColor(sf::Color(170, 30, 25));
	m_pauseText.setOutlineThickness(5.f);
	const sf::FloatRect titleBounds = m_pauseText.getLocalBounds();
	m_pauseText.setOrigin(titleBounds.left + titleBounds.width / 2.f,
	                      titleBounds.top + titleBounds.height / 2.f);
	m_pauseText.setPosition(640.f, 205.f);

	const std::array<const char*, 2> labels = {
		"CONTINUE", "RETURN TO MENU"
	};
	for (std::size_t i = 0; i < m_buttons.size(); ++i) {
		auto& button = m_buttons[i];
		button.shape.setSize({330.f, 54.f});
		button.shape.setOrigin(165.f, 27.f);
		button.shape.setPosition(
			640.f, 325.f + static_cast<float>(i) * 78.f);
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
}

void PausedState::processEvents(sf::Event& event) {
	if (event.type == sf::Event::MouseMoved) {
		const int hovered = buttonAt({
			static_cast<float>(event.mouseMove.x),
			static_cast<float>(event.mouseMove.y)
		});
		if (hovered != m_hoveredButton) {
			m_hoveredButton = hovered;
			if (hovered >= 0) {
				m_selectedButton = hovered;
			}
			updateButtonAppearance();
		}
		return;
	}

	if (event.type == sf::Event::MouseButtonPressed
		&& event.mouseButton.button == sf::Mouse::Left) {
		const int clicked = buttonAt({
			static_cast<float>(event.mouseButton.x),
			static_cast<float>(event.mouseButton.y)
		});
		if (clicked >= 0) {
			m_selectedButton = clicked;
			m_hoveredButton = clicked;
			updateButtonAppearance();
			activateButton(clicked);
		}
		return;
	}

	if (event.type != sf::Event::KeyPressed) return;

	if (event.key.code == sf::Keyboard::Tab) {
		activateButton(0);
		return;
	}
	const int buttonCount = static_cast<int>(m_buttons.size());
	const int activeButton = m_hoveredButton >= 0
		? m_hoveredButton : m_selectedButton;
	if (event.key.code == sf::Keyboard::Up) {
		selectButton((activeButton + buttonCount - 1) % buttonCount);
		return;
	}
	if (event.key.code == sf::Keyboard::Down) {
		selectButton((activeButton + 1) % buttonCount);
		return;
	}
	if (event.key.code == sf::Keyboard::Enter) {
		selectButton(activeButton);
		activateButton(activeButton);
	}
}

void PausedState::update(sf::Time dt) {
    // Game is paused, no physics updates happen here.
}

void PausedState::render(sf::RenderWindow& window) {
	window.setView(window.getDefaultView());
	window.draw(m_fadeOverlay);
    window.draw(m_pauseText);
	for (const auto& button : m_buttons) {
		window.draw(button.shape);
		window.draw(button.label);
	}
}

void PausedState::selectButton(int index) {
	if (index < 0 || index >= static_cast<int>(m_buttons.size())) return;
	const bool appearanceChanged = index != m_selectedButton
		|| m_hoveredButton >= 0;
	m_selectedButton = index;
	m_hoveredButton = -1;
	if (appearanceChanged) updateButtonAppearance();
}

void PausedState::activateButton(int index) {
	if (index == 0) {
		Game::getInstance().popState();
		return;
	}
	if (index == 1) {
		Game::getInstance().clearStatesAndChange(
			std::make_unique<MainMenuState>());
	}
}

int PausedState::buttonAt(sf::Vector2f point) const {
	for (std::size_t i = 0; i < m_buttons.size(); ++i) {
		if (m_buttons[i].shape.getGlobalBounds().contains(point)) {
			return static_cast<int>(i);
		}
	}
	return -1;
}

void PausedState::updateButtonAppearance() {
	const int activeButton = m_hoveredButton >= 0
		? m_hoveredButton : m_selectedButton;
	for (std::size_t i = 0; i < m_buttons.size(); ++i) {
		const bool selected = static_cast<int>(i) == activeButton;
		auto& button = m_buttons[i];
		button.shape.setFillColor(selected
			? sf::Color(245, 195, 45, 235)
			: sf::Color(20, 35, 65, 210));
		button.shape.setOutlineColor(selected
			? sf::Color::White
			: sf::Color(230, 230, 230, 220));
		button.label.setFillColor(selected
			? sf::Color(125, 25, 20)
			: sf::Color::White);
	}
}
