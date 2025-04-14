#pragma once
//#include "Common.h"
#include "Scene.h"
#include <memory>
#include <map>

#include "EntityManager.h"

class Scene_Zelda : public Scene
{
	struct PlayerConfig {
		float ROOMX,ROOMY,X, Y, CX, CY, SPEED,HEALTH;
		std::string WEAPON;
	};

	struct EnemyConfig {
		float ROOMX, ROOMY, X, Y, BLOCKM, BLOCKV,HEALTH,DAMAGE;
		std::string NAME;
	};



protected:

	std::shared_ptr<Entity>		m_player;
	std::string					m_levelPath;
	PlayerConfig				m_playerConfig;
	EnemyConfig					m_enemyConfig;
	bool						m_drawTextures = true;
	bool						m_drawCollision = false;
	bool						m_drawGrid = false;
	bool						m_follow = false;
	const Vec2					m_gridSize = { 64, 64 };
	const Vec2					m_roomSize = { 20, 12 };
	sf::Text					m_gridText;
	Vec2						m_mPos;
	Vec2						m_room = { 0, 0 };
	sf::CircleShape				m_mouseShape;

	void init(const std::string& levelPath);
	Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);
	void loadLevel(const std::string& filename);
	void spawnPlayer();
	void spawnEnemy();
	void spawnSword(std::shared_ptr<Entity> entity);
	void hitBlock(std::shared_ptr<Entity> entity);
	void onEnd();

	Vec2 windowToWorld(const Vec2& window) const;
	Vec2 getPosition(int rx, int ry, int tx, int ty) const;

	void sDragAndDrop();
	void sAI();
	void sStatus();
	void sMovement();
	void sLifespan();
	void sCollision();
	void sDoAction(const Action& action);
	void sAnimation();
	void sCamera();
	void sRender();
public:
	Scene_Zelda();
	Scene_Zelda(GameEngine* gameEngine, const std::string& levelPath);
	void update();

};

