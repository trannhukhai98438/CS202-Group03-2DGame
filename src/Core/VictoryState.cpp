#include "Core/VictoryState.h"

#include "Core/Game.h"
#include "Core/LevelCatalog.h"
#include "Core/MainMenuState.h"
#include "Core/TransitionState.h"
#include "Managers/HUDManager.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <memory>
#include <utility>

namespace {
constexpr char VICTORY_TITLE[] = "VICTORY!";
constexpr float LETTER_STAGGER_SECONDS = 0.10f;
constexpr float LETTER_FLIGHT_SECONDS = 0.58f;
constexpr float BUTTON_REVEAL_DELAY_SECONDS = 0.22f;
constexpr float LETTER_ARC_HEIGHT = 120.f;
constexpr float PI = 3.14159265358979323846f;
constexpr float BUTTON_REVEAL_TIME =
	7.f * LETTER_STAGGER_SECONDS
	+ LETTER_FLIGHT_SECONDS
	+ BUTTON_REVEAL_DELAY_SECONDS;

float easeOutCubic(float value) {
	const float remaining = 1.f - value;
	return 1.f - remaining * remaining * remaining;
}
}

VictoryState::VictoryState(sf::Vector2f launchPosition,
                           std::shared_ptr<HUDManager> hudManager)
	: m_hudManager(std::move(hudManager)),
	  m_launchPosition(launchPosition),
	  m_hasNextLevel(LevelCatalog::nextAfter(
	      Game::getInstance().getSelectedLevelPath()) != nullptr) {
	if (!m_font.loadFromFile("assets/fonts/SuperMario256.ttf")) {
		std::cerr << "ERROR: Failed to load victory font!\n";
	}

	m_fadeOverlay.setSize({1280.f, 720.f});
	m_fadeOverlay.setFillColor(sf::Color(0, 0, 0, 160));

	// Use one laid-out string to retain the font's kerning, then animate each
	// glyph independently from the Hero to its final place in that layout.
	sf::Text titleLayout;
	titleLayout.setFont(m_font);
	titleLayout.setString(VICTORY_TITLE);
	titleLayout.setCharacterSize(68);
	titleLayout.setStyle(sf::Text::Bold);
	const sf::FloatRect titleBounds = titleLayout.getLocalBounds();
	titleLayout.setPosition(
		640.f - (titleBounds.left + titleBounds.width / 2.f),
		175.f - (titleBounds.top + titleBounds.height / 2.f));

	for (std::size_t i = 0; i < m_letters.size(); ++i) {
		auto& letter = m_letters[i];
		letter.glyph.setFont(m_font);
		letter.glyph.setString(sf::String(VICTORY_TITLE[i]));
		letter.glyph.setCharacterSize(68);
		letter.glyph.setStyle(sf::Text::Bold);
		letter.glyph.setFillColor(sf::Color(255, 220, 55));
		letter.glyph.setOutlineColor(sf::Color(170, 30, 25));
		letter.glyph.setOutlineThickness(5.f);

		const sf::FloatRect glyphBounds = letter.glyph.getLocalBounds();
		letter.glyph.setOrigin(
			glyphBounds.left + glyphBounds.width / 2.f,
			glyphBounds.top + glyphBounds.height / 2.f);
		sf::Vector2f characterPosition =
			titleLayout.findCharacterPos(static_cast<unsigned int>(i));
		if (i > 0) {
			characterPosition.x += m_font.getKerning(
				static_cast<sf::Uint32>(VICTORY_TITLE[i - 1]),
				static_cast<sf::Uint32>(VICTORY_TITLE[i]),
				68,
				true);
		}
		letter.targetPosition = {
			characterPosition.x + glyphBounds.left + glyphBounds.width / 2.f,
			characterPosition.y + glyphBounds.top + glyphBounds.height / 2.f
		};
		letter.delay = static_cast<float>(i) * LETTER_STAGGER_SECONDS;
		letter.initialRotation = i % 2 == 0 ? -55.f : 55.f;
	}
	updateLetterAnimation();

	const std::array<const char*, 3> labels = {
		"PLAY AGAIN", "RETURN TO MENU", "NEXT LEVEL"
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

	m_statusText.setFont(m_font);
	m_statusText.setCharacterSize(18);
	m_statusText.setFillColor(sf::Color(255, 220, 55));
	setStatusText("");
}

void VictoryState::processEvents(sf::Event& event) {
	if (!m_buttonsVisible) return;

	if (event.type == sf::Event::MouseMoved) {
		const int hovered = buttonAt({
			static_cast<float>(event.mouseMove.x),
			static_cast<float>(event.mouseMove.y)
		});
		if (hovered != m_hoveredButton) {
			m_hoveredButton = hovered;
			if (hovered >= 0) {
				if (hovered != m_selectedButton) {
					setStatusText("");
				}
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
			setStatusText("");
			updateButtonAppearance();
			activateButton(clicked);
		}
		return;
	}

	if (event.type != sf::Event::KeyPressed) return;

	if (event.key.code == sf::Keyboard::Tab) {
		selectButton(0);
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

void VictoryState::update(sf::Time dt) {
	m_elapsedSeconds += dt.asSeconds();
	updateLetterAnimation();
	if (!m_buttonsVisible && m_elapsedSeconds >= BUTTON_REVEAL_TIME) {
		m_buttonsVisible = true;
	}
}

void VictoryState::render(sf::RenderWindow& window) {
	window.setView(window.getDefaultView());
	window.draw(m_fadeOverlay);

	for (const auto& letter : m_letters) {
		if (m_elapsedSeconds >= letter.delay) {
			window.draw(letter.glyph);
		}
	}

	if (!m_buttonsVisible) return;
	if (!m_initialHoverSynced) {
		const sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
		m_hoveredButton = buttonAt({
			static_cast<float>(mousePosition.x),
			static_cast<float>(mousePosition.y)
		});
		if (m_hoveredButton >= 0) {
			m_selectedButton = m_hoveredButton;
		}
		updateButtonAppearance();
		m_initialHoverSynced = true;
	}
	for (const auto& button : m_buttons) {
		window.draw(button.shape);
		window.draw(button.label);
	}
	window.draw(m_statusText);
}

void VictoryState::selectButton(int index) {
	if (index < 0 || index >= static_cast<int>(m_buttons.size())) return;
	const bool appearanceChanged = index != m_selectedButton
		|| m_hoveredButton >= 0;
	m_selectedButton = index;
	m_hoveredButton = -1;
	setStatusText("");
	if (appearanceChanged) updateButtonAppearance();
}

void VictoryState::activateButton(int index) {
	if (index == 0) {
		Game::getInstance().clearStatesAndChange(
			std::make_unique<TransitionState>());
		return;
	}
	if (index == 1) {
		Game::getInstance().clearStatesAndChange(
			std::make_unique<MainMenuState>());
		return;
	}
	if (index == 2) {
		Game& game = Game::getInstance();
		const LevelCatalog::LevelDefinition* nextLevel =
			LevelCatalog::nextAfter(game.getSelectedLevelPath());
		if (!nextLevel) {
			setStatusText("NO NEXT LEVEL AVAILABLE");
			return;
		}

		if (!m_hudManager) {
			m_hudManager = std::make_shared<HUDManager>();
		}
		m_hudManager->setWorld(nextLevel->hudWorldName);
		m_hudManager->resetTimer();
		game.setSelectedLevel(nextLevel->mapPath, nextLevel->worldName);
		game.clearStatesAndChange(std::make_unique<TransitionState>(
			m_hudManager, nextLevel->mapPath, nextLevel->worldName));
	}
}

void VictoryState::updateButtonAppearance() {
	const int activeButton = m_hoveredButton >= 0
		? m_hoveredButton : m_selectedButton;
	for (std::size_t i = 0; i < m_buttons.size(); ++i) {
		const bool selected = static_cast<int>(i) == activeButton;
		auto& button = m_buttons[i];
		if (i == 2 && !m_hasNextLevel) {
			button.shape.setFillColor(sf::Color(55, 55, 65, 210));
			button.shape.setOutlineColor(selected
				? sf::Color(255, 220, 55)
				: sf::Color(135, 135, 145));
			button.label.setFillColor(sf::Color(160, 160, 170));
			continue;
		}
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

void VictoryState::updateLetterAnimation() {
	for (auto& letter : m_letters) {
		const float progress = std::clamp(
			(m_elapsedSeconds - letter.delay) / LETTER_FLIGHT_SECONDS,
			0.f, 1.f);
		const float easedProgress = easeOutCubic(progress);
		const sf::Vector2f directPosition = {
			m_launchPosition.x
				+ (letter.targetPosition.x - m_launchPosition.x)
				* easedProgress,
			m_launchPosition.y
				+ (letter.targetPosition.y - m_launchPosition.y)
				* easedProgress
		};
		const float arcProgress = std::sin(PI * progress);
		letter.glyph.setPosition(
			directPosition.x,
			directPosition.y
				- arcProgress * arcProgress * LETTER_ARC_HEIGHT);
		letter.glyph.setRotation(
			letter.initialRotation * (1.f - easedProgress));
		const float scale = 0.35f + 0.65f * easedProgress;
		letter.glyph.setScale(scale, scale);
	}
}

void VictoryState::setStatusText(const sf::String& message) {
	m_statusText.setString(message);
	const sf::FloatRect bounds = m_statusText.getLocalBounds();
	m_statusText.setOrigin(bounds.left + bounds.width / 2.f,
	                       bounds.top + bounds.height / 2.f);
	m_statusText.setPosition(640.f, 570.f);
}

int VictoryState::buttonAt(sf::Vector2f point) const {
	for (std::size_t i = 0; i < m_buttons.size(); ++i) {
		if (m_buttons[i].shape.getGlobalBounds().contains(point)) {
			return static_cast<int>(i);
		}
	}
	return -1;
}
