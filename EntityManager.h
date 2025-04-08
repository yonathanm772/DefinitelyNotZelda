#pragma once

#include "Entity.h"
#include <vector>
#include <map>
#include <memory>

typedef std::vector<std::shared_ptr<Entity>> EntityVec;
//typedef std::map<std::string, EntityVec> EntityMap;

class EntityManager
{

	EntityVec							m_entities;
	int count = 0;
	EntityVec							m_entitiesToAdd;
	//EntityVec					m_entitiesToDelete;
	//EntityMap m_entityMap;
	std::map<std::string, EntityVec>	m_entityMap;
	size_t								m_totalEntities = 0;

	void removeDeadEntities(EntityVec& vec);
	void removeDeadEntitiesFromMap();

public:

	EntityManager();

	void update();

	std::shared_ptr<Entity> addEntity( const std::string& tag);

	const EntityVec& getEntities();
	const EntityVec& getEntities(const std::string& tag);

};

