#include "Core/GuideState.h"

#include "Core/Game.h"
#include "Core/MainMenuState.h"

#include <iostream>
#include <memory>

namespace {
constexpr int PAGE_COUNT = 3;

void centerText(sf::Text& text, sf::Vector2f position) {
    const sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.f,
                   bounds.top + bounds.height / 2.f);
    text.setPosition(position);
}
}

GuideState::GuideState() {
    if (!m_font.loadFromFile("assets/fonts/SuperMario256.ttf")) {
        std::cerr << "Error loading guide font\n";
    }
    if (!m_backgroundTexture.loadFromFile("assets/textures/menuBackground.png")) {
        std::cerr << "Error loading guide background\n";
    }
    m_backgroundSprite.setTexture(m_backgroundTexture);
    const sf::Vector2u backgroundSize = m_backgroundTexture.getSize();
    if (backgroundSize.x > 0 && backgroundSize.y > 0) {
        m_backgroundSprite.setScale(1280.f / backgroundSize.x,
                                    720.f / backgroundSize.y);
    }

    m_overlay.setSize({1280.f, 720.f});
    m_overlay.setFillColor(sf::Color(0, 0, 0, 125));
    m_panel.setSize({1040.f, 530.f});
    m_panel.setPosition(120.f, 105.f);
    m_panel.setFillColor(sf::Color(12, 25, 48, 225));
    m_panel.setOutlineColor(sf::Color(235, 235, 235, 220));
    m_panel.setOutlineThickness(3.f);

    m_title.setFont(m_font);
    m_title.setString("GAME GUIDE");
    m_title.setCharacterSize(44);
    m_title.setFillColor(sf::Color(255, 220, 55));
    m_title.setOutlineColor(sf::Color(145, 25, 20));
    m_title.setOutlineThickness(3.f);
    centerText(m_title, {640.f, 67.f});

    m_pageTitle.setFont(m_font);
    m_pageTitle.setCharacterSize(30);
    m_pageTitle.setFillColor(sf::Color(255, 220, 55));

    m_body.setFont(m_font);
    m_body.setCharacterSize(20);
    m_body.setFillColor(sf::Color::White);
    m_body.setLineSpacing(1.45f);

    m_pageIndicator.setFont(m_font);
    m_pageIndicator.setCharacterSize(15);
    m_pageIndicator.setFillColor(sf::Color(210, 215, 225));

    m_marioTexture.loadFromFile("assets/textures/Mario.png");
    m_luigiTexture.loadFromFile("assets/textures/Luigi.png");
	m_flashTexture.loadFromFile("assets/textures/Flash.png");
    m_mushroomTexture.loadFromFile("assets/textures/Mushroom.png");
    m_flowerTexture.loadFromFile("assets/textures/Flower.png");
    m_starTexture.loadFromFile("assets/textures/Star.png");
    m_coinTexture.loadFromFile("assets/textures/Coin.png");
	if (!m_keyboardTexture.loadFromFile("assets/textures/keyboard.png")) {
		std::cerr << "Error loading keyboard texture\n";
	}
    m_marioTexture.setSmooth(false);
    m_luigiTexture.setSmooth(false);
	m_flashTexture.setSmooth(false);
	m_keyboardTexture.setSmooth(false);

    for (auto& label : m_demoLabels) {
        label.setFont(m_font);
        label.setCharacterSize(17);
        label.setFillColor(sf::Color::White);
    }

	// W, Up, A, Left, S, Down, D, Right, X and Tab regions.
	const std::array<sf::IntRect, 10> keyRects = {
		sf::IntRect(289, 324, 94, 105), sf::IntRect(679, 817, 95, 96),
		sf::IntRect(213, 442, 94, 105), sf::IntRect(559, 922, 106, 96),
		sf::IntRect(311, 442, 94, 105), sf::IntRect(676, 922, 98, 96),
		sf::IntRect(409, 442, 94, 105), sf::IntRect(787, 922, 106, 96),
		sf::IntRect(359, 560, 94, 105), sf::IntRect(43, 324, 142, 105)
	};
	const std::array<float, 6> rowY = {
		205.f, 270.f, 335.f, 400.f, 465.f, 530.f
	};
	for (std::size_t i = 0; i < m_keyboardSprites.size(); ++i) {
		auto& key = m_keyboardSprites[i];
		key.setTexture(m_keyboardTexture);
		key.setTextureRect(keyRects[i]);
		key.setOrigin(keyRects[i].width / 2.f, keyRects[i].height / 2.f);
		key.setScale(0.48f, 0.48f);
		const std::size_t row = i < 8 ? i / 2 : i - 4;
		const bool alternativeKey = i < 8 && i % 2 == 1;
		key.setPosition(alternativeKey ? 350.f : 270.f, rowY[row]);
	}

	const std::array<const char*, 6> descriptions = {
		"JUMP", "MOVE LEFT", "CROUCH (GIANT / FIRE)",
		"MOVE RIGHT", "USE SPECIAL POWER", "PAUSE / RESUME"
	};
	for (std::size_t i = 0; i < m_controlDescriptions.size(); ++i) {
		auto& description = m_controlDescriptions[i];
		description.setFont(m_font);
		description.setString(descriptions[i]);
		description.setCharacterSize(17);
		description.setFillColor(sf::Color::White);
		description.setPosition(420.f, rowY[i] - 12.f);
	}
	for (std::size_t i = 0; i < m_orLabels.size(); ++i) {
		auto& separator = m_orLabels[i];
		separator.setFont(m_font);
		separator.setString("OR");
		separator.setCharacterSize(11);
		separator.setFillColor(sf::Color(200, 205, 215));
		centerText(separator, {310.f, rowY[i]});
	}

    const std::array<const char*, 3> buttonLabels = {
        "PREVIOUS", "BACK", "NEXT"
    };
    const std::array<float, 3> buttonX = {310.f, 640.f, 970.f};
    for (std::size_t i = 0; i < m_buttons.size(); ++i) {
        auto& button = m_buttons[i];
        button.shape.setSize({220.f, 52.f});
        button.shape.setOrigin(110.f, 26.f);
        button.shape.setPosition(buttonX[i], 675.f);
        button.shape.setOutlineThickness(3.f);
        button.label.setFont(m_font);
        button.label.setString(buttonLabels[i]);
        button.label.setCharacterSize(19);
        centerText(button.label, button.shape.getPosition());
    }

    showPage(0);
    updateButtonAppearance();
}

void GuideState::processEvents(sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        m_hoveredButton = buttonAt({static_cast<float>(event.mouseMove.x),
                                    static_cast<float>(event.mouseMove.y)});
        updateButtonAppearance();
    }

    if (event.type == sf::Event::MouseButtonPressed
        && event.mouseButton.button == sf::Mouse::Left) {
        const int clicked = buttonAt({static_cast<float>(event.mouseButton.x),
                                      static_cast<float>(event.mouseButton.y)});
        if (clicked == 0 && m_currentPage > 0) {
            showPage(m_currentPage - 1);
        } else if (clicked == 1) {
            returnToMenu();
            return;
        } else if (clicked == 2 && m_currentPage < PAGE_COUNT - 1) {
            showPage(m_currentPage + 1);
        }
    }

    if (event.type == sf::Event::KeyPressed) {
        if ((event.key.code == sf::Keyboard::Left
             || event.key.code == sf::Keyboard::A)
            && m_currentPage > 0) {
            showPage(m_currentPage - 1);
        } else if ((event.key.code == sf::Keyboard::Right
                    || event.key.code == sf::Keyboard::D)
                   && m_currentPage < PAGE_COUNT - 1) {
            showPage(m_currentPage + 1);
        } else if (event.key.code == sf::Keyboard::Escape) {
            returnToMenu();
            return;
        }
    }
}

void GuideState::update(sf::Time dt) {
    (void)dt;
}

void GuideState::render(sf::RenderWindow& window) {
    window.draw(m_backgroundSprite);
    window.draw(m_overlay);
    window.draw(m_panel);
    window.draw(m_title);
    window.draw(m_pageTitle);
    window.draw(m_body);
	if (m_currentPage == 0) {
		for (const auto& key : m_keyboardSprites) {
			window.draw(key);
		}
		for (const auto& description : m_controlDescriptions) {
			window.draw(description);
		}
		for (const auto& separator : m_orLabels) {
			window.draw(separator);
		}
	}
    for (std::size_t i = 0; i < m_demoSprites.size(); ++i) {
        if (m_demoSprites[i].getTexture()) {
            window.draw(m_demoSprites[i]);
            window.draw(m_demoLabels[i]);
        }
    }
    window.draw(m_pageIndicator);
    for (const auto& button : m_buttons) {
        window.draw(button.shape);
        window.draw(button.label);
    }
}

void GuideState::showPage(int page) {
    m_currentPage = page;
    for (auto& sprite : m_demoSprites) {
        sprite = sf::Sprite();
    }
    for (auto& label : m_demoLabels) {
        label.setString("");
    }

    if (page == 0) {
        m_pageTitle.setString("CONTROLS");
        centerText(m_pageTitle, {640.f, 145.f});
		m_body.setString("");

        m_demoSprites[0].setTexture(m_marioTexture);
        m_demoSprites[0].setTextureRect({0, 8, 16, 16});
		m_demoSprites[0].setScale(6.f, 6.f);
		m_demoSprites[0].setPosition(940.f, 300.f);
		m_demoLabels[0].setString("READY!");
		centerText(m_demoLabels[0], {985.f, 425.f});
    } else if (page == 1) {
        m_pageTitle.setString("POWER-UPS");
        centerText(m_pageTitle, {640.f, 145.f});
        m_body.setString(
            "MUSHROOM   GROW INTO GIANT FORM\n\n\n"
			"FLOWER     UNLOCK SPECIAL (MUSHROOM REQUIRED)\n\n\n"
            "STAR       TEMPORARY INVINCIBILITY\n\n\n"
            "COIN       100 COINS GIVE AN EXTRA LIFE");
        m_body.setPosition(420.f, 215.f);

        const std::array<sf::Texture*, 4> textures = {
            &m_mushroomTexture, &m_flowerTexture, &m_starTexture, &m_coinTexture
        };
        const std::array<sf::IntRect, 4> rects = {
            sf::IntRect(0, 0, 16, 16), sf::IntRect(1, 0, 16, 16),
            sf::IntRect(1, 1, 14, 16), sf::IntRect(1, 1, 8, 14)
        };
        const std::array<float, 4> yPositions = {220.f, 310.f, 400.f, 490.f};
        for (std::size_t i = 0; i < m_demoSprites.size(); ++i) {
            m_demoSprites[i].setTexture(*textures[i]);
            m_demoSprites[i].setTextureRect(rects[i]);
            m_demoSprites[i].setScale(4.f, 4.f);
            m_demoSprites[i].setPosition(285.f, yPositions[i]);
        }
    } else {
        m_pageTitle.setString("CHARACTERS");
        centerText(m_pageTitle, {640.f, 145.f});
		m_body.setString("");

        m_demoSprites[0].setTexture(m_marioTexture);
        m_demoSprites[0].setTextureRect({0, 8, 16, 16});
		m_demoSprites[0].setScale(7.f, 7.f);
		m_demoSprites[0].setPosition(235.f, 245.f);
        m_demoSprites[1].setTexture(m_luigiTexture);
        m_demoSprites[1].setTextureRect({0, 8, 16, 16});
        m_demoSprites[1].setScale(7.f, 7.f);
		m_demoSprites[1].setPosition(575.f, 245.f);
		m_demoSprites[2].setTexture(m_flashTexture);
		m_demoSprites[2].setTextureRect({15, 52, 104, 113});
		m_demoSprites[2].setScale(0.9f, 0.9f);
		m_demoSprites[2].setPosition(895.f, 235.f);

		m_demoLabels[0].setString(
			"MARIO\nBOUNCING FIREBALL\nCOOLDOWN: 2 SECONDS");
		centerText(m_demoLabels[0], {290.f, 445.f});
		m_demoLabels[1].setString(
			"LUIGI\nWATER BOMB + SPLASH\nCOOLDOWN: 3 SECONDS");
		centerText(m_demoLabels[1], {630.f, 445.f});
		m_demoLabels[2].setString(
			"FLASH\nFAST THUNDER BOLT\nCOOLDOWN: 2.5 SECONDS");
		centerText(m_demoLabels[2], {950.f, 445.f});
    }

    m_pageIndicator.setString(std::to_string(page + 1) + " / "
                              + std::to_string(PAGE_COUNT));
    centerText(m_pageIndicator, {640.f, 610.f});
    updateButtonAppearance();
}

void GuideState::updateButtonAppearance() {
    for (std::size_t i = 0; i < m_buttons.size(); ++i) {
        const bool unavailable = (i == 0 && m_currentPage == 0)
            || (i == 2 && m_currentPage == PAGE_COUNT - 1);
        const bool hovered = static_cast<int>(i) == m_hoveredButton
            && !unavailable;
        auto& button = m_buttons[i];
        button.shape.setFillColor(unavailable
            ? sf::Color(35, 40, 50, 190)
            : hovered ? sf::Color(245, 195, 45, 235)
                      : sf::Color(25, 55, 90, 230));
        button.shape.setOutlineColor(unavailable
            ? sf::Color(100, 105, 115) : sf::Color::White);
        button.label.setFillColor(unavailable
            ? sf::Color(115, 120, 130)
            : hovered ? sf::Color(125, 25, 20) : sf::Color::White);
    }
}

int GuideState::buttonAt(sf::Vector2f point) const {
    for (std::size_t i = 0; i < m_buttons.size(); ++i) {
        if (m_buttons[i].shape.getGlobalBounds().contains(point)) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void GuideState::returnToMenu() {
    Game::getInstance().changeState(std::make_unique<MainMenuState>());
}
