#include "Scene_Zelda.h"
//#include "Common.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
//#include "Action.h"

#include <fstream>
#include <iostream>
//#include "Scene_Menu.h"


Scene_Zelda::Scene_Zelda()
{
}

Scene_Zelda::Scene_Zelda(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine)
	, m_levelPath(levelPath)
{
	init(m_levelPath);
	
}


void Scene_Zelda::init(const std::string& levelPath)
{
	std::cout << "LevelPath: " << levelPath << "\n";
	loadLevel(levelPath);

	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::Y, "TOGGLE_FOLLOW");
	registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
	registerAction(sf::Keyboard::G, "TOGGLE_GRID");
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::Space, "ATTACK");

	m_gridText.setCharacterSize(12);
	m_gridText.setFont(m_game->assets().getFont("Tech"));
	std::cout << "Window Pos" << m_game->window().getPosition().x << "\n";
	std::cout << "Window Pos" << m_game->window().getPosition().y << "\n";
	std::cout << "Window Size" << m_game->window().getSize().x << "\n";
	std::cout << "Window Size" << m_game->window().getSize().y << "\n";

	
	

}


void Scene_Zelda::loadLevel(const std::string& filename)
{

	m_entityManager = EntityManager();
	//TODO:
	// Use the getPosition() function below to convert room-tile coords to game world coords 
	// similar to midToGridPixel
	
	std::cout << "Loading From File" << "\n";

	std::ifstream fin(filename);
	if (!fin) {
		std::cerr << "Error opening file!" << std::endl;
	}
	std::string assetType, asset;
	float xCoord = 0.0f,
		yCoord = 0.0f;
	float  roomX, roomY, posX, posY, blocksM, blocksV;
	roomX = roomY = posX = posY = blocksM = blocksV = 0.0f;

	while (!fin.eof())
	{
		fin >> assetType;
		if (assetType == "Tile") {
			fin >> asset >> roomX >> roomY >> posX >> posY >> blocksM >> blocksV;

			auto tile = m_entityManager.addEntity("tile");
			//IMPORTANT: always add the CAnimation component first so that gridToMidPixel can compute correctly
			tile->addComponent<CAnimation>(m_game->assets().getAnimation(asset), true);
			tile->addComponent<CTransform>(getPosition(roomX,roomY,posX,posY));
			tile->addComponent<CBoundingBox>(m_game->assets().getAnimation(asset).getSize(), blocksM, blocksV);
			tile->addComponent<CDraggable>();

		}
		else if (assetType == "Player") {
			fin >> m_playerConfig.ROOMX >> m_playerConfig.ROOMY >>m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CX
				>> m_playerConfig.CY >> m_playerConfig.SPEED >> m_playerConfig.HEALTH;
			spawnPlayer();

		}
		else if (assetType == "NPC") {
			fin >> m_enemyConfig.NAME >> m_enemyConfig.ROOMX >> m_enemyConfig.ROOMY >> m_enemyConfig.X >> m_enemyConfig.Y >> m_enemyConfig.BLOCKM
				>> m_enemyConfig.BLOCKV >> m_enemyConfig.HEALTH >> m_enemyConfig.DAMAGE;

			spawnEnemy();

		}
	}
	fin.close();
	std::cout << "Finished Loading From File" << "\n";
	
}


Vec2 Scene_Zelda::getPosition(int rx, int ry, int tx, int ty) const
{
	// Total tile index in world space
	int globalTileX = rx * m_roomSize.x + tx;
	int globalTileY = ry * m_roomSize.y + ty;

	// Bottom-left corner of that tile in pixels
	int pixelX = globalTileX * m_gridSize.x;
	int pixelY = (globalTileY) * m_gridSize.y;

	// Center it
	float centerX = pixelX + m_gridSize.x / 2.0f;
	float centerY = pixelY + m_gridSize.y / 2.0f;

	return Vec2(centerX, centerY);
}

void Scene_Zelda::spawnPlayer()
{
	if (m_player) { m_player->destroy(); }
	std::cout << "Spawning Player:" << "\n";
	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandDown"), true);
	m_player->addComponent<CTransform>(getPosition(m_playerConfig.ROOMX, m_playerConfig.ROOMX,
												   m_playerConfig.X, m_playerConfig.Y));
	m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY), true, false);
	m_player->addComponent<CHealth>(m_playerConfig.HEALTH, m_playerConfig.HEALTH);
}

void Scene_Zelda::spawnSword(std::shared_ptr<Entity> entity)
{
	auto sword = m_entityManager.addEntity("sword");

	auto& transform = entity->getComponent<CTransform>();
	Vec2 facing = transform.facing;

	std::string swordAnim;
	// Base position = player's position
	Vec2 swordPos = transform.pos;

	if (facing.x != 0) // Horizontal: use SwordRight
	{
		swordPos.x += m_gridSize.x * facing.x;
	}
	else if (facing.y != 0) // Vertical: use SwordUp
	{
		swordPos.y += m_gridSize.y * facing.y;
	}

	// Add transform component
	sword->addComponent<CTransform>(swordPos, Vec2(0, 0), facing, 0);
	sword->addComponent<CAnimation>(m_game->assets().getAnimation("SwordUp"), true);
	sword->addComponent<CBoundingBox>(m_game->assets().getAnimation("SwordUp").getSize());
	sword->addComponent<CLifespan>(15, (int)m_currentFrame);
	sword->addComponent<CDamage>(1);

	// Play slash sound
	//m_game->playSound("Slash");
}

void Scene_Zelda::update()
{
	m_entityManager.update();
	//TODO: implement pause functionality
	if (!m_paused)
	{
		sAI();
		sMovement();
		sStatus();
		sLifespan();
		sCollision();
		sAnimation();
		sCamera();
		sDragAndDrop();

		m_currentFrame++;
	}

}

void Scene_Zelda::sMovement()
{
	auto& input = m_player->getComponent<CInput>();
	auto& transform = m_player->getComponent<CTransform>();
	auto& state = m_player->getComponent<CState>();
	auto& animation = m_player->getComponent<CAnimation>();
	auto& sprite = m_player->getComponent<CAnimation>().animation.getSprite();
	Vec2 playerInputSpeed(0, 0);

	


	if (input.up && (!input.down))
	{
		if(state.state != "attacking")	{ state.state = "runUp"; }
		transform.facing = { 0,-1 };
		playerInputSpeed.y -= m_playerConfig.SPEED;
		

	}
	else if (input.down && (!input.up))
	{
		if (state.state != "attacking") { state.state = "runDown"; }
		transform.facing = { 0, 1};
		playerInputSpeed.y += m_playerConfig.SPEED;

	}
	else if (input.left && (!input.right))
	{
		if (state.state != "attacking") { state.state = "runX"; }
		transform.facing = { -1,0 };
		playerInputSpeed.x -= m_playerConfig.SPEED;
		transform.scale.x = -1;
	}
	else if (input.right && (!input.left))
	{
		if (state.state != "attacking") { state.state = "runX"; }
		transform.facing = { 1, 0};
		playerInputSpeed.x += m_playerConfig.SPEED;
		transform.scale.x = 1;
	}

	if (input.attack && input.canAttack)
	{
		spawnSword(m_player);
		state.state = "attacking";
		input.attack = false;
		input.canAttack = false;
	}

	transform.velocity = playerInputSpeed;

	for (auto e : m_entityManager.getEntities())
	{
		auto& transformE = e->getComponent<CTransform>();
		auto& sprite = e->getComponent<CAnimation>().animation.getSprite();
		auto& boundingBox = e->getComponent<CBoundingBox>();
		float left = boundingBox.halfSize.x;

		transformE.prevPos = transformE.pos;
		transformE.pos += transformE.velocity;
		
		

	}

}

void Scene_Zelda::sDoAction(const Action& action)
{

	if (action.type() == "START")
	{
		auto& pState = m_player->getComponent<CState>();
		auto& pTransform = m_player->getComponent<CTransform>();
		auto& pInput = m_player->getComponent<CInput>();

		if (action.name() == "TOGGLE_TEXTURE")			{ m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION")	{ m_drawCollision = !m_drawCollision; }
		else if (action.name() == "TOGGLE_FOLLOW")		{ m_follow = !m_follow; }
		else if (action.name() == "TOGGLE_GRID")		{ m_drawGrid = !m_drawGrid; }
		else if (action.name() == "PAUSE")				{ m_paused = !m_paused; }
		else if (action.name() == "QUIT")				{ onEnd(); }
		else if (action.name() == "UP")
		{
			pInput.up = true;
		}
		else if (action.name() == "DOWN")
		{
			pInput.down = true;
		}
		else if (action.name() == "LEFT")
		{
			pInput.left = true;
		}
		else if (action.name() == "RIGHT")
		{
			pInput.right = true;
		}
		else if (action.name() == "ATTACK" && pInput.canAttack)
		{
			pInput.attack = true;
		}
		else if (action.name() == "LEFT_CLICK")
		{
			Vec2 worldPos = windowToWorld(action.pos());

			for (auto e : m_entityManager.getEntities())
			{
				if (e->hasComponent<CDraggable>() && Physics::IsInside(worldPos, e))
				{
					//std::cout << "Clicked Entity: " << e->getComponent<CAnimation>().animation.getName() << "\n";
					//e->getComponent<CDraggable>().dragging = !e->getComponent<CDraggable>().dragging;
					//std::cout << "Dragging?: " << e->getComponent<CDraggable>().dragging << "\n";

					auto& draggable = e->getComponent<CDraggable>();
					draggable.dragging = !draggable.dragging;

					std::cout << "Clicked Entity: " << e->getComponent<CAnimation>().animation.getName() << "\n";
					std::cout << "Dragging?: " << draggable.dragging << "\n";

				}
			}
		}
		else if (action.name() == "MOUSE_MOVE")
		{
			m_mPos = action.pos();
			m_mouseShape.setPosition(m_mPos.x, m_mPos.y);
		}
	}
	else if (action.type() == "END")
	{
		auto& pTransform = m_player->getComponent<CTransform>();
		auto& pInput = m_player->getComponent<CInput>();
		auto& pState = m_player->getComponent<CState>();
		if (action.name() == "UP")
		{
			pInput.up = false;
		}
		else if (action.name() == "DOWN")
		{
			pInput.down = false;
		}
		else if (action.name() == "LEFT")
		{
			pInput.left = false;
		}
		else if (action.name() == "RIGHT")
		{
			pInput.right = false;
			//pState.state = "run";
		}
		else if (action.name() == "ATTACK")
		{
			pInput.attack = false;
			//pInput.canAttack = true;
		}
	}
}

void Scene_Zelda::sAI()
{
	// TODO
	// Implement the Enemy AI
	// Implement Follow behavior
	// Implement Patrol behavior
	// loop over all the entities, do they have a patrol component?
		// if they do, do this
	//do they have a follow component ?
		// if they do, do this
	// for each entity, figure out the line segment ab between the player and the entity
	//		then, for each entity that blocks vision, call entity intersect on it
}

void Scene_Zelda::sStatus()
{
	// TODO
	// Implement Lifespan Here
	// Implement Invincibility Frames here
	// 
	// whenever the player is not invincible, and it gets hit, attach an invincibility component
	// for a certain duration
}

void Scene_Zelda::sCollision()
{
	// TODO
	// Implement player - enemy collisions with the appropriate damage calculations
	// Implement sword - npc collisions
	// Implement entity - heart collisions and life gain logic
	// Implement black tile collisions / 'teleporting' tp another black tile
	// May want to use helper functions for these behaviors or this function will get long
	// like hitBlock();

	auto& tiles = m_entityManager.getEntities("tile");
	auto& enemies = m_entityManager.getEntities("enemy");
	auto& swords = m_entityManager.getEntities("sword");

	auto& transform = m_player->getComponent<CTransform>();
	auto& state = m_player->getComponent<CState>();

	for (auto sword : swords)
	{
		if (!sword->isActive() || !sword->hasComponent<CDamage>()) continue;
		std::cout << "Spawned sword ID: " << sword->id() << "\n";
		for (auto enemy : enemies)
		{
			if (!enemy->hasComponent<CBoundingBox>()) { continue; }
			
			Vec2 overlap = Physics::getOverlap(sword, enemy);
			if (overlap.x > 0 && overlap.y > 0)
			{
				
				std::cout << "Health Before DMG: " << enemy->getComponent<CHealth>().current << "\n";
				enemy->getComponent<CHealth>().current -= sword->getComponent<CDamage>().damage;
				std::cout << "Health After DMG: " << enemy->getComponent<CHealth>().current << "\n";
				std::cout << "Has DMG COMPONENT================================: " << sword->getComponent<CDamage>().has << "\n";
				sword->removeComponent<CDamage>();
				std::cout << "Has DMG COMPONENT================================: " << sword->getComponent<CDamage>().has << "\n";
				std::cout << "Checking sword ID: " << sword->id()
					<< ", has CDamage: " << sword->hasComponent<CDamage>() << "\n";
				break;

				/*if (enemy->getComponent<CAnimation>().animation.getName() == "Tektite")
				{
					tile->addComponent<CAnimation>(m_game->assets().getAnimation("Explosion"), true);
					tile->removeComponent<CBoundingBox>();
					tile->getComponent<CAnimation>().repeat = false;
				}*/

			}
		}
	}

	for (auto e : m_entityManager.getEntities("player"))
	{
		bool isColliding = false;  // Track if the player collides with any tile


		for (auto t : tiles)
		{
			Vec2 overlap = Physics::getOverlap(e, t);
			Vec2 prevOverlap = Physics::getPreviousOverlap(e, t);

			Vec2 shift(0, 0);  // Position correction
			Vec2 diff = transform.pos - t->getComponent<CTransform>().pos; // Difference in position
			auto& block = t->getComponent<CBoundingBox>();
			if (overlap.x > 0 && overlap.y > 0)  // Ensure valid collision
			{
				isColliding = true;  // Player is colliding with at least one tile
				if (!block.blockMove) { continue; }
				// Handle different collision directions
				// bottom collision
				if (prevOverlap.x > 0 && transform.prevPos.y < t->getComponent<CTransform>().prevPos.y)
				{

					transform.pos.y -= overlap.y;
					transform.velocity.y = 0;
					std::cout << "Velocity in Bottom collision " << transform.velocity.y << "\n";

					// Check if the tile is moving (a platform)

				}
				//top collision
				else if (prevOverlap.x > 0 && transform.prevPos.y > t->getComponent<CTransform>().prevPos.y)
				{
					transform.pos.y += overlap.y;
					transform.velocity.y = 0;
					std::cout << "Velocity in Top collision " << transform.velocity.y << "\n";



					//hitBlock(t);

				}
				// if there was a non-zero previous X overlap, the the collision came from x
				else if (prevOverlap.y > 0)
				{
					std::cout << "Horizontal\n";
					shift.x += diff.x > 0 ? overlap.x : -overlap.x;

					transform.velocity.x = 0;
					std::cout << "Velocity in Side collision " << transform.velocity.x << "\n";

				}
				transform.pos += shift;
			}
		}

	}

	// does not let the player exit the screen to the left
	/*if (m_player->getComponent<CTransform>().pos.x < m_player->getComponent<CBoundingBox>().halfSize.x)
	{
		m_player->getComponent<CTransform>().pos.x = m_player->getComponent<CBoundingBox>().halfSize.x;
	}*/

}

void Scene_Zelda::sAnimation()
{
	// TODO
	// Implement player facing direction animation
	// Implement sword animation based on player facing
	// The sword should move if the player changes direction mid swing'
	// Implement destruction of entities with non-repeating finished animations
	auto& currentAnimation = m_player->getComponent<CAnimation>().animation;
	auto& state = m_player->getComponent<CState>();
	auto& pTransform = m_player->getComponent<CTransform>();
	auto& input = m_player->getComponent<CInput>();



	bool swordExists = false;
	for (auto e : m_entityManager.getEntities("sword"))
	{
		if (e->isActive()) {
			swordExists = true;
			break;
		}
	}
	if (state.state == "runX")
	{
		if ((input.left || input.right) && !(input.left && input.right))
		{
			if (currentAnimation.getName() != "RunRight")
			{
				m_player->addComponent<CAnimation>(m_game->assets().getAnimation("RunRight"), true);
			}
		}
		else if (input.left && input.right)
		{
			m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandDown"), true);
		}
		else
		{
			m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandRight"), true);
			m_player->getComponent<CAnimation>().animation.getSprite().setScale(pTransform.facing.x, pTransform.facing.y);
		}
	}
	else if (state.state == "runUp")
	{
		if ((input.up) && !(input.up && input.down))
		{
			if (currentAnimation.getName() != "RunUp")
			{
				m_player->addComponent<CAnimation>(m_game->assets().getAnimation("RunUp"), true);
			}
		}
		else if (input.up && input.down)
		{
			m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandDown"), true);
		}
		else
		{

			auto prevScale = m_player->getComponent<CAnimation>().animation.getSprite();
			m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandUp"), true);
			m_player->getComponent<CAnimation>().animation.getSprite().setScale(prevScale.getScale());
		}
	}
	else if (state.state == "runDown")
	{
		if ((input.down) && !(input.up && input.down))
		{
			if (currentAnimation.getName() != "RunDown")
			{
				m_player->addComponent<CAnimation>(m_game->assets().getAnimation("RunDown"), true);
			}
		}
		else
		{	
			auto prevScale = m_player->getComponent<CAnimation>().animation.getSprite();
			
			m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandDown"), true);
			m_player->getComponent<CAnimation>().animation.getSprite().setScale(prevScale.getScale());
		}
	}
	else if (state.state == "attacking" && swordExists)
	{
		if (pTransform.facing.y > 0)
		{
			if (currentAnimation.getName() != "AtkDown")
			{

				m_player->addComponent<CAnimation>(m_game->assets().getAnimation("AtkDown"), true);
			}
		}
		else if (pTransform.facing.y < 0)
		{
			if (currentAnimation.getName() != "AtkUp")
			{

				m_player->addComponent<CAnimation>(m_game->assets().getAnimation("AtkUp"), true);
			}
		}
		else if (pTransform.facing.x != 0)
		{
			Vec2 scale = pTransform.facing;

			(pTransform.facing.x == 1) ? scale.x = pTransform.facing.x : scale.x = -1;
			if (currentAnimation.getName() != "AtkRight")
			{

				m_player->addComponent<CAnimation>(m_game->assets().getAnimation("AtkRight"), true);
				pTransform.facing = scale;
			}
		}
	}
	else
	{
		std::string playerAnim = "StandDown";
		Vec2 scale = { 1, -1 };  // default
		if (pTransform.facing.x != 0) // Horizonta
		{
			playerAnim = "StandRight";
			scale.x = pTransform.facing.x;
		}
		else if (pTransform.facing.y != 0) // Vertical
		{
			(pTransform.facing.y == scale.y) ? playerAnim = "StandUp" : playerAnim = "StandDown";
			scale.y = pTransform.facing.y;
		}

		m_player->addComponent<CAnimation>(m_game->assets().getAnimation(playerAnim), true);
	}

	for (auto e : m_entityManager.getEntities())
	{
		if (!e->hasComponent<CAnimation>()) { continue; }

		auto& cAnimation = e->getComponent<CAnimation>();
		if (e->tag() == "sword")
		{
			Vec2 facing = pTransform.facing;
			std::string swordAnim;
			Vec2 scale = { 1, -1 };  // default
			// Base position = player's position
			Vec2 swordPos = pTransform.pos;

			if (facing.x != 0) // Horizontal: use SwordRight
			{
				swordAnim = "SwordRight";
				swordPos.x += m_gridSize.x * facing.x;
				scale.x = facing.x;
			}
			else if (facing.y != 0) // Vertical: use SwordUp
			{
				swordAnim = "SwordUp";
				swordPos.y += m_gridSize.y * facing.y;
				scale.y *= facing.y;
			}
			e->addComponent<CAnimation>(m_game->assets().getAnimation(swordAnim), true);
			e->getComponent<CTransform>().pos = pTransform.facing * m_gridSize.x + pTransform.pos;
			e->getComponent<CTransform>().scale = scale;


			
		}
		// if the animation is not repeating and it is completed, destroy the entity
		if (cAnimation.animation.hasEnded() && !cAnimation.repeat)
		{
			e->destroy();
		}
		else
		{
			cAnimation.animation.update();
		}

	}
}

void Scene_Zelda::sCamera()
{
	// TODO:
	// Implemnet camera view logic
	// 
	// Get the current view, which we will modify in the if-statement below
	//



	sf::View view = m_game->window().getView();
	auto& pPos = m_player->getComponent<CTransform>().pos;
	Vec2 pWorldPos = windowToWorld(pPos);

	if (m_follow)
	{
		// calculate view for player follow camera
		view.setCenter(sf::Vector2f(m_player->getComponent<CTransform>().pos.x,
									m_player->getComponent<CTransform>().pos.y));
	}
	else
	{

		if (pPos.x > width())
		{
			m_room.x = 1;
		}
		else if (pPos.x < width() && pPos.x >= 0)
		{
			m_room.x = 0;
		}
		else if (pPos.x < width() && pPos.x <= 0)
		{
			m_room.x = -1;
		}

		if (pPos.y > height())
		{
			m_room.y = 1;
		}
		else if (pPos.y < height() && pPos.y >= 0)
		{
			m_room.y = 0;
		}
		else if (pPos.y < height() && pPos.y <= 0)
		{
			m_room.y = -1;
		}
		// calculate view for room-based camera
		view.reset(sf::FloatRect(m_room.x * width(), m_room.y * height(), width(), height()));
	}

	// then set the window view
	m_game->window().setView(view);
}

Vec2 Scene_Zelda::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{

	// TODO: This function takes in a grid (x,y) position and an entity
	//		Return a Vec2 indicating where the CENTER position of the entity should be
	//		You must use the Entity's Animation size to position it correctly
	//		The size of the grid width and height is stored in m_gridSize.x and m_gridSize.y
	//		The bottom-left corner of the Animation should align with the bottom left of the grid cell

	int bottom_Left_CoordX_Grid = gridX * m_gridSize.x;
	//int bottom_Left_CoordY_Grid = height() - (gridY * m_gridSize.y);
	int bottom_Left_CoordY_Grid = (gridY + 1) * m_gridSize.y;
	Vec2 size;
	if (entity->hasComponent<CAnimation>())
	{
		size = entity->getComponent<CAnimation>().animation.getSize();
		
	}
	std::cout << size.x << " " << size.y << "\n";
	// Return the position adjusted for the grid and animation center
	return Vec2(bottom_Left_CoordX_Grid + size.x / 2, bottom_Left_CoordY_Grid - size.y / 2);
}




void Scene_Zelda::spawnEnemy()
{
	std::cout << "Spawning Enemy:" << "\n";
	//here is a sample player entity which you can use to construct other entities
	auto entity = m_entityManager.addEntity("enemy");
	entity->addComponent<CAnimation>(m_game->assets().getAnimation("Tektite"), true);
	entity->addComponent<CTransform>(getPosition(m_enemyConfig.ROOMX, m_enemyConfig.ROOMX,
		m_enemyConfig.X, m_enemyConfig.Y));
	entity->addComponent<CBoundingBox>(m_game->assets().getAnimation("SwordUp").getSize(), m_enemyConfig.BLOCKM, m_enemyConfig.BLOCKV);
	entity->addComponent<CHealth>(m_enemyConfig.HEALTH, m_enemyConfig.HEALTH);
	entity->addComponent<CDamage>(m_enemyConfig.DAMAGE);
}


/*
void Scene_Zelda::SpawnBullet(std::shared_ptr<Entity> entity)
{
		
		auto b = m_entityManager.addEntity("bullet");
		auto& transform = entity->getComponent<CTransform>();
		auto& boundingBox = entity->getComponent<CBoundingBox>();

		b->addComponent<CTransform>(transform.pos, Vec2(transform.scale.x * 12, 0), transform.scale,0);
		b->addComponent<CAnimation>(m_game->assets().getAnimation(m_playerConfig.WEAPON), true);
		b->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_playerConfig.WEAPON).getSize()/2);
		b->addComponent<CLifespan>(60,(int)m_currentFrame);

auto b = m_entityManager.addEntity("bullet");
		auto& transform = entity->getComponent<CTransform>();
		auto& boundingBox = entity->getComponent<CBoundingBox>();

		b->addComponent<CTransform>(transform.pos, Vec2(transform.scale.x * 12, 0), transform.scale,0);
		b->addComponent<CAnimation>(m_game->assets().getAnimation(m_playerConfig.WEAPON), true);
		b->addComponent<CBoundingBox>(m_game->assets().getAnimation(m_playerConfig.WEAPON).getSize()/2);
		b->addComponent<CLifespan>(60,(int)m_currentFrame)
*/
/*void Scene_Zelda::hitBlock(std::shared_ptr<Entity> entity)
{
	// Ensure the block has an animation component
	if (!entity->hasComponent<CAnimation>())
		return;

	auto& animation = entity->getComponent<CAnimation>();

	if (animation.animation.getName() == "Brick")
	{
		entity->addComponent<CAnimation>(m_game->assets().getAnimation("Explosion"), true);
		entity->removeComponent<CBoundingBox>();
		animation.repeat = false;
	}
	else if (animation.animation.getName() == "Question")
	{

		auto coin = m_entityManager.addEntity("coin");
		coin->addComponent<CAnimation>(m_game->assets().getAnimation("Coin"), true);
		coin->addComponent<CTransform>(entity->getComponent<CTransform>().pos + Vec2(0, -64)); // Position coin above the block
		coin->addComponent<CLifespan>(30, (int)m_currentFrame); // Destroy the coin after 1 second

		entity->addComponent<CAnimation>(m_game->assets().getAnimation("Question2"), true);
		
	}
}*/

void Scene_Zelda::sLifespan()
{
	
	// TODO: Check lifespawn of entities that have them, and destroy them if the go over
	// Loop through all entities
	//
	auto& pInput = m_player->getComponent<CInput>();
	auto& pState = m_player->getComponent<CState>().state;
	bool swordAlive = false;

	for (auto e : m_entityManager.getEntities())
	{
		auto& lifespan = e->getComponent<CLifespan>();
		// Check if the entity has a lifespan component
		if (lifespan.has)
		{
			if (e->tag() == "sword")
			{
				pInput.canAttack = false;
				swordAlive = true;
			}
			// If lifespan is still remaining, decrease it
			if (lifespan.lifespan > 0)
			{
				lifespan.lifespan -= 1;
			}

			// If lifespan has expired, deactivate the entity
			if (lifespan.lifespan <= 0)
			{
				e->destroy();
				pState = "stand";
			}
		}
	}

	if (!swordAlive && !pInput.attack) {
		pInput.canAttack = true;
	}
}


void Scene_Zelda::sDragAndDrop()
{
	for (auto e : m_entityManager.getEntities())
	{
		if (e->hasComponent<CDraggable>() && e->getComponent<CDraggable>().dragging)
		{
			Vec2 worldPos = windowToWorld(m_mPos);
			e->getComponent<CTransform>().pos = worldPos;
		}
	}
}



void Scene_Zelda::onEnd()
{
	//TODO:
	// Implement Scene end
	// - stop the music
	// - play the menu music
	// -change scene to menu
	//
	m_hasEnded = true;
	m_game->changeScene("MENU", nullptr, true);
}

void Scene_Zelda::sRender()
{

	// color the background darker so you know that the game is paused
	if (!m_paused)
		m_game->window().clear(sf::Color(255, 192, 122));
	else
		m_game->window().clear(sf::Color(100, 100, 255));

	// set the viewport of the window to be cented on the player if its far enough right
	/*auto& pPos = m_player->getComponent<CTransform>().pos;
	float windowCenterX = std::max(m_game->window().getSize().x / 2.0f, pPos.x);
	sf::View view = m_game->window().getView();
	view.setCenter(windowCenterX, m_game->window().getSize().y - view.getCenter().y);
	m_game->window().setView(view);*/
	sf::RectangleShape thick({ 1.0f, 6.0f });
	thick.setFillColor(sf::Color::Black);


	//draw all Entity textures / animations 
	if (m_drawTextures)
	{
		for (auto e : m_entityManager.getEntities())
		{
			auto& transform = e->getComponent<CTransform>();
			sf::Color c = sf::Color::White;

			if (e->hasComponent<CInvincibility>())
			{
				c = sf::Color(255, 255, 255, 128);
			}

			if (e->hasComponent<CAnimation>())
			{
				auto& animation = e->getComponent<CAnimation>().animation;
				animation.getSprite().setRotation(transform.angle);
				animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
				animation.getSprite().setScale(transform.scale.x, transform.scale.y);
				animation.getSprite().setColor(c);
				m_game->window().draw(animation.getSprite());
			}
		}

		for (auto e : m_entityManager.getEntities())
		{
			auto& transform = e->getComponent<CTransform>();

			if (e->hasComponent<CHealth>())
			{
				auto &h = e->getComponent<CHealth>();
				Vec2 size(64, 6);
				sf::RectangleShape rect({ size.x, size.y });
				rect.setPosition(transform.pos.x - 32, transform.pos.y - 48);
				rect.setFillColor(sf::Color(96, 96, 96));
				rect.setOutlineColor(sf::Color::Black);
				rect.setOutlineThickness(2);
				m_game->window().draw(rect);

				float ratio = (float)(h.current) / h.max;
				size.x *= ratio;
				rect.setSize({ size.x, size.y });
				rect.setFillColor(sf::Color(255, 0, 0));
				rect.setOutlineThickness(0);
				m_game->window().draw(rect);

				for (int i = 0; i < h.max; i++)
				{
					thick.setPosition(rect.getPosition() + sf::Vector2f(i * 64 * 1 / h.max, 0));
					m_game->window().draw(thick);
				}
			}
		}

		
	}

	//draw the grid so that students can easily debug
	if (m_drawGrid)
	{
		float leftX = m_game->window().getView().getCenter().x - width() / 2;
		float rightX = leftX + width() + m_gridSize.x;
		float topY = m_game->window().getView().getCenter().y - height() / 2;
		float bottomY = topY + height() + m_gridSize.y;
		float nextGridX = leftX - ((int)leftX % (int)m_gridSize.x);
		float nextGridY = topY - ((int)topY % (int)m_gridSize.y);

		for (float x = nextGridX; x < rightX; x += m_gridSize.x)
		{
			drawLine(Vec2(x, topY), Vec2(x, bottomY));
		}

		for (float y = nextGridY; y < bottomY; y += m_gridSize.y)
		{

			drawLine(Vec2(leftX, y), Vec2(rightX, y));

			for (float x = nextGridX; x < rightX; x += m_gridSize.x)
			{

				std::string xCell = std::to_string((int)x / (int)m_gridSize.x);
				std::string yCell = std::to_string((int)y / (int)m_gridSize.y);
				m_gridText.setString("(" + xCell + "," + yCell + ")");
				m_gridText.setPosition(x + 3, y + 2);
				m_game->window().draw(m_gridText);
			}

		}
	}

	// draw all entity collision bounding boxes with rectangleshape
	if (m_drawCollision)
	{
		sf::CircleShape dot(4);
		dot.setFillColor(sf::Color::Black);
		for (auto e : m_entityManager.getEntities())
		{
			if (e->hasComponent<CBoundingBox>())
			{
				
				auto& box = e->getComponent<CBoundingBox>();
				auto& transform = e->getComponent <CTransform>();
				sf::RectangleShape rect;
				rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
				rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
				rect.setPosition(transform.pos.x, transform.pos.y);
				rect.setFillColor(sf::Color(0, 0, 0, 0));

				if(box.blockMove && box.blockVision) { rect.setOutlineColor(sf::Color::Black); }
				if (box.blockMove && !box.blockVision) { rect.setOutlineColor(sf::Color::Blue); }
				if (!box.blockMove && box.blockVision) { rect.setOutlineColor(sf::Color::Red); }
				if (!box.blockMove && box.blockVision) { rect.setOutlineColor(sf::Color::White); }
				rect.setOutlineThickness(1);
				m_game->window().draw(rect);
			}

			if (e->hasComponent<CPatrol>())
			{
				auto& patrol = e->getComponent<CPatrol>().positions;
				for (size_t p = 0; p < patrol.size(); p++)
				{
					dot.setPosition(patrol[p].x, patrol[p].y);
					m_game->window().draw(dot);
				}
			}

			if (e->hasComponent<CFollowPlayer>())
			{
				sf::VertexArray lines(sf::LinesStrip, 2);
				lines[0].position.x = e->getComponent<CTransform>().pos.x;
				lines[0].position.y = e->getComponent<CTransform>().pos.y;
				lines[0].color = sf::Color::Black;
				lines[1].position.x = m_player->getComponent<CTransform>().pos.x;
				lines[1].position.y = m_player->getComponent<CTransform>().pos.y;
				lines[1].color = sf::Color::Black;
				m_game->window().draw(lines);
				dot.setPosition(e->getComponent<CFollowPlayer>().home.x, e->getComponent<CFollowPlayer>().home.y);
				m_game->window().draw(dot);
			}

		}
	}

	m_mouseShape.setFillColor(sf::Color::Red);
	m_mouseShape.setRadius(4);
	m_mouseShape.setOrigin(2, 2);
	
	Vec2 worldPos = windowToWorld(m_mPos);
	m_mouseShape.setPosition(worldPos.x, worldPos.y);
	m_game->window().draw(m_mouseShape);
}

// wont work if we zoom in or out, have a minimap etc, need to recalculate
Vec2 Scene_Zelda::windowToWorld(const Vec2& window) const
{
	auto view = m_game->window().getView();

	float worldX = view.getCenter().x - (m_game->window().getSize().x / 2);
	float worldY = view.getCenter().y - (m_game->window().getSize().y / 2);

	return Vec2(window.x + worldX, window.y + worldY);
}

//sCollision system
