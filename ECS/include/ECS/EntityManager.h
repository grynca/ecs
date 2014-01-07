/*
 * EntityManager.h
 *
 *  Created on: 19. 12. 2013
 *      Author: lutza
 */

#ifndef ENTITYMANAGER_H_
#define ENTITYMANAGER_H_

#include <map>
#include <cassert>
#include "Components.h"
#include "Systems.h"
#include "Entity.h"

namespace Grynca
{
	class EntityManager
	{
		template<typename Derived, typename... Comps> friend class System;
	public:
		EntityManager();
		virtual ~EntityManager();

		// TODO: some way at construction to register systems and components

		// adds new entity
		//	!!! dont store ptr or ref to Entity, it can get invalidated when removing entities
		//	for persistent storage use entity->guid();
		Entity* addEntity();

		// usage: em.deleteEntity(em.getEntity(guid))
		void deleteEntity(Entity* entity);

		// returns NULL if entity does not exist
		//	!! is not meant for persistent storage (can be invalidated when removing entities) !!
		//		( use guid instead )
		Entity* getEntity(uint32_t guid);

		// add component to entity
		//	it sets component mask in entity and returns ptr to where component data should be set
		uint8_t* addComponent(Entity* entity, unsigned int comp_family_id);
		// adds component and also constructs it with component's constructor
		template <typename CompType, typename ... Args>
		CompType* addComponent(Entity* entity, Args && ... args);

		// removes component from entity mask
		void removeComponent(Entity* entity, unsigned int comp_family_id);
		template <typename CompType>
		void removeComponent(Entity* entity);

		template <typename CompType>
		CompType* getComponent(Entity* entity);

		BaseSystem* getSystem(unsigned int system_id);
		template <typename SysType>
		SysType& getSystem();

		unsigned int entitiesCount();
		unsigned int registeredComponentsCount();
		// TODO: registeredSystemsCount()
	protected:
		// TODO:
		virtual void _registerComponents() = 0;
		virtual void _registerEntities() = 0;
		virtual void _registerSystems() = 0;


		void _doubleCapacity();


		struct EntityDataPos
		{
			uint32_t pool_id;	// type id of entity
			uint32_t pool_pos;	// where are data for entity's components in pool
		};

		// mapping from global entity id to EntityDataPos
		std::map<uint32_t, EntityDataPos> _entities_pos_map;


		Components _components;
		Systems	   _systems;

		std::vector<Entity> _entities;
		// maps entity guids to local array indices
		std::map<uint32_t, uint32_t> _entity_guid_to_local_id;
		uint32_t _last_guid;
	};

}

template <typename CompType, typename ... Args>
inline CompType* Grynca::EntityManager::addComponent(Entity* entity, Args && ... args)
{
	uint8_t* comp_data = addComponent(entity, CompType::familyId());
	// construct component with placement new
	new(comp_data)CompType(args...);
	return (CompType*)comp_data;
}


template <typename CompType>
inline void Grynca::EntityManager::removeComponent(Entity* entity)
{
	removeComponent(entity, CompType::familyId());
}

template <typename CompType>
inline CompType* Grynca::EntityManager::getComponent(Grynca::Entity* entity)
{
	assert(entity);
	if (entity->containsComponent(CompType::familyId()))
		return _components.get<CompType>(entity->_local_id);
	else
		// entity does not contain that component
		return NULL;
}

template <typename SysType>
inline SysType& Grynca::EntityManager::getSystem()
{
	return *(SysType*)getSystem(SysType::systemId());
}

#endif /* ENTITYMANAGER_H_ */
