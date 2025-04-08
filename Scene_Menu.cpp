#include "Scene_Menu.h"
#include "GameEngine.h"
#include "Scene_Zelda.h"
#include "Assets.h"
#include <iostream>

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
	:Scene(gameEngine)
{
	for (auto& action : m_actionMap)
	{
		std::cout << "Action: " << action.second << " mapped to key " << action.first << "\n";
	}
	init();
}

void Scene_Menu::init()
{
	m_menuTexts.clear();
	std::cout << "Menu Init";
	m_title = "Definetly Not Zelda";
	m_displayText = { "LEVEL 1", "LEVEL 2", "LEVEL 3"};
	m_levelPaths = { "level1", "level2", "level3" };
	m_menuFont = m_game->assets().getFont("Megaman");

	
	//m_menuSound = m_game->assets().getSound("MusicTitle");
	//m_game->playSound("MusicTitle");

	sf::Text textTitle;
	textTitle.setFont(m_menuFont);
	textTitle.setString(m_title);
	textTitle.setCharacterSize(50);
	textTitle.setPosition(10, 10);
	textTitle.setFillColor(sf::Color(128, 128, 128));
	m_menuTexts.push_back(textTitle);

	m_controls = "UP:W	DOWN:S	PLAY:D	BACK: ESC";
	sf::Text textControls;
	textControls.setFont(m_menuFont);
	textControls.setString(m_controls);
	textControls.setPosition(10, (m_game->window().getSize().y) - 60);
	textControls.setCharacterSize(20);
	textControls.setFillColor(sf::Color(128, 128, 128));
	m_menuTexts.push_back(textControls);
	

	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::D, "PLAY");
	registerAction(sf::Keyboard::Escape, "QUIT");

	for (size_t i = 0; i < m_displayText.size(); i++)
	{
		sf::Text text;
		text.setFont(m_menuFont);
		text.setString(m_displayText[i]);
		text.setCharacterSize(50);
		text.setPosition(10, 120 + (i * 60));
		text.setFillColor(i == m_selectedMenuIndex ? sf::Color::White : sf::Color(128, 128, 128));
		m_menuTexts.push_back(text);
	}
}

void Scene_Menu::update()
{
	
}


void Scene_Menu::onEnd()
{
	m_game->quit();
	
}

void Scene_Menu::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "UP")
		{
			
			if (m_selectedMenuIndex > 0)
			{
				m_selectedMenuIndex--;
			}
			else
				m_selectedMenuIndex = m_displayText.size() - 1;
			
		}
		else if (action.name() == "DOWN")
		{
			m_selectedMenuIndex = (m_selectedMenuIndex + 1) % m_displayText.size();
		}
		else if (action.name() == "PLAY")
		{
			
			m_game->changeScene("PLAY", std::make_shared<Scene_Zelda>(m_game, m_levelPaths[m_selectedMenuIndex] + ".txt"), true);
			m_hasEnded = true;
			
		}
		else if (action.name() == "QUIT")
		{
			onEnd();
		}

	}
	for (size_t i = 0; i < m_menuTexts.size(); i++)
	{
		if (m_menuTexts[m_selectedMenuIndex+1].getString() != m_title ||
			m_menuTexts[m_selectedMenuIndex + 1].getString() != m_controls)
		{
			m_menuTexts[i].setFillColor((i == m_selectedMenuIndex+2) ? sf::Color::White : sf::Color(128, 128, 128));
		}
	}
}

void Scene_Menu::sRender()
{
	//std::cout << "Rendering";
	m_game->window().clear(sf::Color::Black);
	for (const auto& text : m_menuTexts)
	{
		m_game->window().draw(text);
	}

	// changing scenes https://github.com/eXpl0it3r/SmallGameEngine/blob/master/src/MenuState.cpp
}

// play sound music
// implement movement