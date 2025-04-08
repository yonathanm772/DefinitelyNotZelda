/*#pragma once

#include "Entity.h"
#include "EntityManager.h"
#include <random>
#include <SFML/Graphics.hpp>

struct WindowConfig { int WIDTH, HEIGHT, FRAMES, FULLSCREEN; };
struct FontConfig { std::string FONTFILE;int SIZE, FR, FG, FB; };
struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI, SMIN, SMAX; };
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };

class Game
{
	sf::RenderWindow	m_window;		// the window we will draw to
	EntityManager		m_entities;		// vector of entities to maintain
	sf::Font			m_font;			// the font we will use to draw
	sf::Text			m_text;			// the score text to be drawn to the screen
	WindowConfig		m_windowConfig;
	FontConfig			m_fontConfig;
	PlayerConfig		m_playerConfig;
	EnemyConfig			m_enemyConfig;
	BulletConfig		m_bulletConfig;
	int					m_score = 0;
	int					m_currentFrame = 0;
	int					m_lastEnemySpawnTime = 0;
	bool				m_paused = false;
	bool				m_running = true;

	std::shared_ptr<Entity> m_player;

	void init(const std::string& config);// initialize the GameState with a config file path 
	void setPaused(bool paused);

	// Systems
	void sMovement();
	void sUserInput();
	void sLifespan();
	void sRender();
	void sEnemySpawner();
	void sCollision();

	void spawnPlayer();
	void spawnEnemy();
	void spawnSmallEnemies(std::shared_ptr<Entity> entity);
	void spawnBullet(std::shared_ptr<Entity> entity, const Vec2 &mousePos);
	void spawnSpecialWeapon(const Vec2& mousePos);
	Game();

public:


	Game(const std::string& config); // constructor, takes in game config

	void run();

};*/