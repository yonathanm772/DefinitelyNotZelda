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
			tile->addComponent<CTransform>(gridToMidPixel(posX, posY, tile));
			//tile->addComponent<CTransform>(Vec2(posX, posY));
			tile->addComponent<CBoundingBox>(m_game->assets().getAnimation(asset).getSize(), blocksM, blocksV);
			tile->addComponent<CDraggable>();
			//NOTE: Your final code should position the entity with the grid x,y position rtead from the file:

		}
		/*else if (assetType == "Dec") {

			fin >> asset >> xCoord >> yCoord;

			auto dec = m_entityManager.addEntity("dec");
			//IMPORTANT: always add the CAnimation component first so that gridToMidPixel can compute correctly
			dec->addComponent<CAnimation>(m_game->assets().getAnimation(asset), true);
			dec->addComponent<CTransform>(gridToMidPixel(xCoord, yCoord, dec));
			std::cout << "+++++++++++++++DEC NAME: " << dec->getComponent<CAnimation>().animation.getName() << "\n";
			std::cout << "+++++++++++++++DEC COORD: " << dec->getComponent<CTransform>().pos.x << ", " << dec->getComponent<CTransform>().pos.y << "\n";
			dec->addComponent<CDraggable>();
			//NOTE: Your final code should position the entity with the grid x,y position rtead from the file:

			//std::cout << "Loaded Animation:" << assetPath << "\n";
		}*/
		else if (assetType == "Player") {
			fin >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CX
				>> m_playerConfig.CY >> m_playerConfig.SPEED >> m_playerConfig.HEALTH;
			spawnPlayer();

		}
		/*else if (assetType == "Enemy") {
			fin >> m_enemyConfig.X >> m_enemyConfig.Y >> m_enemyConfig.CX
				>> m_enemyConfig.CY >> m_enemyConfig.SPEED >> m_playerConfig.HEALTH;

			//spawnEnemy();

		}*/
	}
	fin.close();
	std::cout << "Finished Loading From File" << "\n";
	
}


Vec2 Scene_Zelda::getPosition(int rx, int ry, int tx, int ty) const
{
	// TODO:
	// Implement this function, which takes in the room (rx, ry) coordinate
	// as well as the tile (tx, ty) coordinate, and returns the Vec2 game world
	// position of the center of the entity

	return Vec2(0,0);
}

void Scene_Zelda::spawnPlayer()
{
	if (m_player) { m_player->destroy(); }
	std::cout << "Spawning Player:" << "\n";
	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandDown"), true);
	m_player->addComponent<CTransform>(Vec2(640,393));
	m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY), true, false);
	m_player->addComponent<CHealth>(m_playerConfig.HEALTH, m_playerConfig.HEALTH);

	std::cout << m_player->getComponent<CTransform>().pos.x<< " " << m_player->getComponent<CTransform>().pos.y << "\n";
}

void Scene_Zelda::spawnSword(std::shared_ptr<Entity> entity)
{
	// TODO:
	// Implement the spawning of the sword, which:
	// - should be given the appropriate lifespan
	// - should spawn at the appropriate location based on player's facing direction
	// - be given a damage value of 1
	// - should play the sound "Slash"
	//

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
		//sLifespan();
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
		state.state = "runUp";
		transform.facing = { 0,-1 };
		playerInputSpeed.y -= m_playerConfig.SPEED;
		

	}
	else if (input.down && (!input.up))
	{
		state.state = "runDown";
		transform.facing = { 0, 1};
		playerInputSpeed.y += m_playerConfig.SPEED;

	}
	else if (input.left && (!input.right))
	{
		state.state = "runX";
		transform.facing = { -1,0 };
		playerInputSpeed.x -= m_playerConfig.SPEED;
		transform.scale.x = -1;
		//if 
	}
	else if (input.right && (!input.left))
	{
		state.state = "runX";
		transform.facing = { 1, 0};
		playerInputSpeed.x += m_playerConfig.SPEED;
		transform.scale.x = 1;
		//if (state.isGrounded) state.state = "ground";
	}

	/*if (input.attack && input.attack)
	{


		//SpawnBullet(m_player);
		//input.attack = false;
	}*/
	transform.velocity = playerInputSpeed;

	/*if (std::abs(playerInputSpeed.x) < 0.01f)
	{
		state.state = "run";
	}*/

	for (auto e : m_entityManager.getEntities())
	{
		auto& transformE = e->getComponent<CTransform>();
		auto& sprite = e->getComponent<CAnimation>().animation.getSprite();
		auto& boundingBox = e->getComponent<CBoundingBox>();
		float left = boundingBox.halfSize.x;
		//float scaleX = std::abs(sprite.getScale().x);

		/*if (transformE.velocity.x > 0 && transformE.scale.x != 1)//transformE.lastDirection != 1)
		{

			//transformE.lastDirection = 1;
			//sprite.setScale(scaleX, sprite.getScale().y);  // Face right
			transformE.scale.x = 1;
			sprite.setScale(transformE.scale.x, transformE.scale.y);

		}
		else if (transformE.velocity.x < 0 && transformE.scale.x != -1)//transformE.lastDirection != -1)
		{
			//transformE.lastDirection = -1;
			//sprite.setScale(-scaleX, sprite.getScale().y); // Face left
			transformE.scale.x = -1;
			sprite.setScale(transformE.scale.x, transformE.scale.y);

		}
		if (e->tag() == "enemy")
		{
			if (transformE.pos.x < boundingBox.halfSize.x)
			{
				transformE.pos.x = left;
				transformE.velocity.x *= -1;

			}
		}*/


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
		else if (action.name() == "ATTACK")
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
			//pState.state = "run";
		}
		else if (action.name() == "RIGHT")
		{
			pInput.right = false;
			//pState.state = "run";
		}
		else if (action.name() == "BUSTER")
		{
			pInput.attack = false;
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

	//auto& tiles = m_entityManager.getEntities("tile");
	/*auto& bullets = m_entityManager.getEntities("bullet");

	auto& transform = m_player->getComponent<CTransform>();
	auto& state = m_player->getComponent<CState>();

	for (auto bullet : bullets)
	{
		for (auto tile : tiles)
		{
			if (!tile->hasComponent<CBoundingBox>()) { continue; }

			Vec2 overlap = Physics::getOverlap(bullet, tile);
			if (overlap.x > 0 && overlap.y > 0)
			{
				bullet->destroy();

				if (tile->getComponent<CAnimation>().animation.getName() == "Brick")
				{
					tile->addComponent<CAnimation>(m_game->assets().getAnimation("Explosion"), true);
					tile->removeComponent<CBoundingBox>();
					tile->getComponent<CAnimation>().repeat = false;
				}

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

			if (overlap.x > 0 && overlap.y > 0)  // Ensure valid collision
			{
				isColliding = true;  // Player is colliding with at least one tile

				// Handle different collision directions
				// bottom collision
				if (prevOverlap.x > 0 && transform.prevPos.y < t->getComponent<CTransform>().prevPos.y)
				{

					transform.pos.y -= overlap.y;
					transform.velocity.y = 0;
					//state.state = "ground";
					state.isGrounded = true;
					e->getComponent<CInput>().canJump = true;

					// Check if the tile is moving (a platform)

				}
				//top collision
				else if (prevOverlap.x > 0 && transform.prevPos.y > t->getComponent<CTransform>().prevPos.y)
				{
					std::cout << "CURRENT STATE: " << state.state << "\n";
					transform.pos.y += overlap.y;
					transform.velocity.y = 0;



					hitBlock(t);

				}
				// if there was a non-zero previous X overlap, the the collision came from x
				else if (prevOverlap.y > 0)
				{
					std::cout << "Horizontal\n";
					shift.x += diff.x > 0 ? overlap.x : -overlap.x;
				}
				transform.pos += shift;
			}
		}
		// If the player is NOT colliding with anything, set the state to "jumping"
		if (!isColliding)
		{
			state.state = "air";
		}

	}

	// check to see if the player has fallen down a hole
	if (m_player->getComponent<CTransform>().pos.y > height())
	{
		spawnPlayer();
	}

	// does not let the player exit the screen to the left
	if (m_player->getComponent<CTransform>().pos.x < m_player->getComponent<CBoundingBox>().halfSize.x)
	{
		m_player->getComponent<CTransform>().pos.x = m_player->getComponent<CBoundingBox>().halfSize.x;
	}
	*/

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

	if (state.state == "runX")
	{
		auto& input = m_player->getComponent<CInput>();
		std::cout << "Left: " << input.left << ", Right: " << input.right << "\n";
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
			std::cout << "Facing " << pTransform.facing.x << "\n";
			std::cout << "Facing " << pTransform.facing.y << "\n";
			m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandRight"), true);
			m_player->getComponent<CAnimation>().animation.getSprite().setScale(pTransform.facing.x, pTransform.facing.y);
			std::cout << "Current " << m_player->getComponent<CAnimation>().animation.getSprite().getScale().x << "\n";
			std::cout << "Current " << m_player->getComponent<CAnimation>().animation.getSprite().getScale().y << "\n";
		}
	}
	else if (state.state == "runUp")
	{
		auto& input = m_player->getComponent<CInput>();
		std::cout << "Up: " << input.up << "\n";
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

			std::cout << "BOTH\n";
			auto prevScale = m_player->getComponent<CAnimation>().animation.getSprite();
			std::cout << "Prev " << prevScale.getScale().x << "\n";
			std::cout << "Prev " << prevScale.getScale().y << "\n";
			m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandUp"), true);
			m_player->getComponent<CAnimation>().animation.getSprite().setScale(prevScale.getScale());
			std::cout << "Current " << m_player->getComponent<CAnimation>().animation.getSprite().getScale().x << "\n";
			std::cout << "Current " << m_player->getComponent<CAnimation>().animation.getSprite().getScale().y << "\n";
		}
	}
	else if (state.state == "runDown")
	{
		auto& input = m_player->getComponent<CInput>();
		std::cout << "Up: " << input.up << "\n";
		if ((input.down) && !(input.up && input.down))
		{
			if (currentAnimation.getName() != "RunDown")
			{
				m_player->addComponent<CAnimation>(m_game->assets().getAnimation("RunDown"), true);
			}
		}
		else
		{

			std::cout << "BOTH\n";
			auto prevScale = m_player->getComponent<CAnimation>().animation.getSprite();
			std::cout << "Prev " << prevScale.getScale().x << "\n";
			std::cout << "Prev " << prevScale.getScale().y << "\n";
			m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandDown"), true);
			m_player->getComponent<CAnimation>().animation.getSprite().setScale(prevScale.getScale());
			std::cout << "Current " << m_player->getComponent<CAnimation>().animation.getSprite().getScale().x << "\n";
			std::cout << "Current " << m_player->getComponent<CAnimation>().animation.getSprite().getScale().y << "\n";
		}
	}
	else
	{
		auto prevScale = currentAnimation.getSprite().getScale();
		m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandDown"), true);
		m_player->getComponent<CAnimation>().animation.getSprite().setScale(prevScale);
	}

	for (auto e : m_entityManager.getEntities())
	{
		if (!e->hasComponent<CAnimation>()) { continue; }

		auto& cAnimation = e->getComponent<CAnimation>();

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

	/*
	// Store the current scale
	auto& currentAnimation = m_player->getComponent<CAnimation>().animation;
	auto& state = m_player->getComponent<CState>();
	auto& pTransform = m_player->getComponent<CTransform>();
	//sf::Vector2f currentScale = currentAnimation.getSprite().getScale();

	if (state.state == "air")
	{
		m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Air"), true);
	}
	else if (state.state == "ground")
	{
		auto& input = m_player->getComponent<CInput>();
		if ((input.left || input.right) && !(input.left && input.right))
		{
			if (currentAnimation.getName() != "Run")
			{
				m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Run"), true);
			}
		}
		else
		{
			m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
		}
		//m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
		//m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
	}
	else
	{
		m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
	}

	for (auto e : m_entityManager.getEntities())
	{
		if (!e->hasComponent<CAnimation>()) { continue; }

		auto& cAnimation = e->getComponent<CAnimation>();

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

	*/
}

void Scene_Zelda::sCamera()
{
	// TODO:
	// Implemnet camera view logic
	// 
	// Get the current view, which we will modify in the if-statement below
	//
	/*
	sf::View view = m_game->window().getView();

	if (m_follow)
	{
		// calculate view for player follow camera
	}
	else
	{
		// calculate view for room-based camera
	}

	// then set the window view
	m_game->window().setView(view);
	*/
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
	/*std::cout << "Spawning Enemy:" << "\n";
	//here is a sample player entity which you can use to construct other entities
	auto entity = m_entityManager.addEntity("enemy");
	entity->addComponent<CAnimation>(m_game->assets().getAnimation("GoombaWalk"), true);
	entity->addComponent<CTransform>(gridToMidPixel(m_enemyConfig.X, m_enemyConfig.Y, entity), Vec2(m_enemyConfig.SPEED,0));
	entity->addComponent<CBoundingBox>(Vec2(m_enemyConfig.CX, m_enemyConfig.CY));
	entity->addComponent<CGravity>(m_enemyConfig.GRAVITY);
	entity->addComponent<CInput>();
	*/
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

}
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
	/**
	// TODO: Check lifespawn of entities that have them, and destroy them if the go over
	// Loop through all entities
	//auto bullets = m_entityManager.getEntities("bullet");
	for (auto e : m_entityManager.getEntities())
	{
		auto& lifespan = e->getComponent<CLifespan>();
		// Check if the entity has a lifespan component
		if (lifespan.has)
		{
			if (e->tag() == "bullet")
			{
				m_player->getComponent<CInput>().canShoot = false;
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
			}
		}
	}*/
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

