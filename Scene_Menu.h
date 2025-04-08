#pragma once
#include "Scene.h"
#include <map>
#include <memory>
#include <deque>
#include "Assets.h"
#include "EntityManager.h"

class Scene_Menu : public Scene
{

protected:

	std::string					m_title;
	std::string					m_controls;
	std::vector<sf::Text>		m_menuTexts;
	std::vector<std::string>	m_levelPaths;
	std::vector<std::string>	m_displayText;
	sf::Font					m_menuFont;
	sf::Sound					m_menuSound;
	size_t						m_selectedMenuIndex = 0;
	int m_selectedIndex = 0;

	void init();
	void update();
	void onEnd();
	void sDoAction(const Action& action);

public:

	Scene_Menu(GameEngine* gameEngine = nullptr);
	void sRender();

};

