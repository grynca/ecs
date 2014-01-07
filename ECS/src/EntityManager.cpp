/*
 * EntityManager.h
 *
 *  Created on: 22. 12. 2013
 *      Author: lutze
 */

#include "ECS/EntityManager.h"
#include <iostream>

Grynca::EntityManager::EntityManager()
 : _components(ENTITIES_INITIAL_CAPACITY),
   _systems(*this),
   _last_guid(0)
{
	_entities.reserve(ENTITIES_INITIAL_CAPACITY);
}

Grynca::EntityManager::~EntityManager()
{
	for (unsigned int ent_id=0; ent_id<_entities.size(); ++ent_id)
	{
		Entity& e = _entities[ent_id];
		// delete components of entities
		for (unsigned int comp_family=0; comp_family<MAX_COMPONENTS; comp_family++)
		{
			if (e._components_mask[comp_family])
			{
				uint8_t* deleted_comp_ptr = _components.getData(comp_family, ent_id);
				_components._destructors[comp_family](deleted_comp_ptr);
			}
		}

	}
}


Grynca::Entity* Grynca::EntityManager::addEntity()
{
	if (_entities.capacity() == _entities.size())
	// pump up new memory if necessary
		_doubleCapacity();

	uint32_t e_guid = _last_guid;
	uint32_t e_local_id = _entities.size();
	// add new entity
	_entities.push_back(Entity(e_guid, e_local_id));
	++_last_guid;
	// TODO: broadcast add component event
	return &_entities.back();
}


void Grynca::EntityManager::deleteEntity(Grynca::Entity* entity)
{
	if (!entity)
	{
#ifndef NDEBUG
		std::cerr << "[EntityManager] Warning: trying to delete non existant entity." << std::endl;
#endif
		return;
	}

	unsigned int deleted_local_id = entity->localIndex();
	unsigned int last_local_id = _entities.size()-1;
	Entity& deleted_entity_slot = _entities[deleted_local_id];
	for (unsigned int comp_family=0; comp_family<MAX_COMPONENTS; comp_family++)
	// explicitely call destructors for components of deleted entity
	{
		if (deleted_entity_slot._components_mask[comp_family])
		{
			uint8_t* deleted_comp_ptr = _components.getData(comp_family, deleted_local_id);
			_components._destructors[comp_family](deleted_comp_ptr);
		}
	}

	if (deleted_local_id != last_local_id )
	// deleting non-last entity
	{
		// swap last entity with deleted entity
		Entity& last_entity_slot = _entities[last_local_id];
		deleted_entity_slot._guid = last_entity_slot._guid;
		deleted_entity_slot._components_mask = last_entity_slot._components_mask;
		deleted_entity_slot._systems_mask = last_entity_slot._systems_mask;
		// move components data for last entity to correct place
		for (unsigned int comp_family=0; comp_family<MAX_COMPONENTS; comp_family++)
		{
			if (last_entity_slot._components_mask[comp_family])
			{
				uint8_t* deleted_comp_data = _components.getData(comp_family, deleted_local_id);
				uint8_t* last_comp_data = _components.getData(comp_family, last_local_id);
				unsigned int comp_size = _components.componentSize(comp_family);
				memcpy(deleted_comp_data, last_comp_data, comp_size);
			}
		}
		// change mapping from guid to local_id for moved entity
		_entity_guid_to_local_id[last_entity_slot._guid] = deleted_local_id;
	}

	// remove mapping for removed entity's guid
	_entity_guid_to_local_id.erase(entity->_guid);
	// delete last Entity struct
	_entities.pop_back();
}

Grynca::Entity* Grynca::EntityManager::getEntity(uint32_t guid)
{
	std::map<uint32_t, uint32_t>::iterator it = _entity_guid_to_local_id.find(guid);
	if (it==_entity_guid_to_local_id.end())
		return NULL;
	else
		return &_entities[it->second];
}

uint8_t* Grynca::EntityManager::addComponent(Entity* entity, unsigned int comp_family_id)
{
	assert(entity);
	assert(comp_family_id < _components.registeredComponentsCount()
			&& "Component not registered.");
	// add component to entity mask
	entity->_components_mask[comp_family_id] = 1;
	// return pointer to component data
	return _components.getData(comp_family_id, entity->localIndex());
}

void Grynca::EntityManager::removeComponent(Entity* entity, unsigned int comp_family_id)
{
	assert(entity);
	assert(comp_family_id < _components.registeredComponentsCount()
		   && "Component not registered.");
	// removes flag from components mask
	entity->_components_mask[comp_family_id] = 0;
}

Grynca::BaseSystem* Grynca::EntityManager::getSystem(unsigned int system_id)
{
	assert((int)system_id != -1
			&& "System is not registered.");
	assert(system_id < _systems.systemsCount());
	return _systems.get(system_id);
}

unsigned int Grynca::EntityManager::entitiesCount()
{
	return _entities.size();
}

unsigned int Grynca::EntityManager::registeredComponentsCount()
{
	return _components._data.size();
}

void Grynca::EntityManager::_doubleCapacity()
{
	unsigned int current_capacity = _entities.capacity();
	unsigned int desired_capacity = current_capacity*2;

	// preallocate space for entities
	_entities.reserve(desired_capacity);
	// for components (already has doubling scheme implemented inside)
	_components._doubleCapacity();
}



