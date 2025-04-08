#include "EntityManager.h"
#include "Entity.h"
#include <iostream>


EntityManager::EntityManager()
{
}

void EntityManager::update()
{

	//  Adss entities from m_entitiesToAdd the proper location(s)
	//	add them to the vector of all entities
	//	add them to the vector inside the map, with the tag as a key
	for (auto e : m_entitiesToAdd)
	{
		m_entities.push_back(e);
		m_entityMap[e->tag()].push_back(e);
	}

	// remove dead entities from the vector of all entities
	
	removeDeadEntities(m_entities);
	removeDeadEntitiesFromMap();

	m_entitiesToAdd.clear();
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string &tag)
{
	auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
	m_entitiesToAdd.push_back(entity);
	return entity;
}

const EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag)
{
	return m_entityMap[tag];
}



void EntityManager::removeDeadEntities(EntityVec& vec)
{
	vec.erase(std::remove_if(vec.begin(), vec.end(),
		[](std::shared_ptr<Entity>& e) { return !e->isActive(); }),
		vec.end());

}

void EntityManager::removeDeadEntitiesFromMap()
{
	for (auto& keyValue : m_entityMap)
	{
		removeDeadEntities(keyValue.second);
	}
}





