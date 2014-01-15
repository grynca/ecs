/*
 * Entities.h
 *
 *  Created on: 8. 1. 2014
 *      Author: lutza
 */

#ifndef ENTITIES_H_
#define ENTITIES_H_

#include "ComponentsRegister.h"

namespace Grynca
{

	// TODO: broadcast events at construction and destruction of entities
	// TODO: broadcast events on add and remove components
	//


	/// contains entities of some specific type
	class Entities
	{
	/// internal structures
		struct EntityPool
		{
			// mask of components that are contained in all entities of this type
			ComponentsMask components_mask;
			SystemsMask initial_update_mask;		// system mask of newly created entities

			ComponentsPool components_pool;			// components data

			std::vector<Entity> entities;
		};
		struct EntityPos
		{
			uint32_t pool_id;	// type id of entity
			uint32_t local_id;	// local index in pool (can change when deleting entities)
		};

		template<typename Derived, typename... Comps> friend class System;
	public:
		Entities(ComponentsRegister& comps_reg)
		 	 : _comps_reg(comps_reg),
		 	   _last_guid(0)
		{
		}

		template <typename EntType>
		void registerEntityType(unsigned int initial_capacity)
		{
			assert(EntType::entityTypeId() == -1
					&& "Entity type is already registered.");

			EntType::_entityTypeId() = _entity_pools.size();
			_entity_pools.push_back( EntityPool() );
			// set components mask
			_entity_pools.back().components_mask = EntType::componentsMask();
			// set initial systems mask
			_entity_pools.back().initial_update_mask = EntType::initialSystemsMask();

			// allocate pool for components to initial size
			_entity_pools.back().components_pool.init(_comps_reg, EntType::componentsMask(), initial_capacity);
			// reserve space for entities
			_entity_pools.back().entities.reserve(initial_capacity);
		}

		unsigned int registeredEntitiesCount()
		{
			return _entity_pools.size();
		}

		// which components do these entities consist of
		const ComponentsMask& componentsMask(unsigned int entity_type_id)
		{
			assert(entity_type_id < _entity_pools.size());
			return _entity_pools[entity_type_id].components_mask;
		}

		// initial update mask for entities of specified type
		const ComponentsMask& initialUpdateMask(unsigned int entity_type_id)
		{
			assert(entity_type_id < _entity_pools.size());
			return _entity_pools[entity_type_id].initial_update_mask;
		}

		/// !! STORE ONLY GUID PERSISTENTLY !!
		///		addEntity(unsigned int entity_type_id).guid()

		// creates new entity of desired type ( does not initialize components )
		Entity* addEntity(unsigned int entity_type_id)
		{
			assert(entity_type_id < _entity_pools.size());

			EntityPool& pool = _entity_pools[entity_type_id];

			if (pool.entities.capacity() == pool.entities.size())
			// pump up new memory if necessary
			{
				// double the capacity
				unsigned int current_capacity = pool.entities.capacity();
				unsigned int doubled_capacity = current_capacity *2;

				pool.entities.reserve(doubled_capacity);
				pool.components_pool.doubleCapacity();
			}

			// add new position mapping
			EntityPos& newent_pos = _entities_pos_map[_last_guid];
			newent_pos->pool_id = entity_type_id;
			newent_pos->local_id = pool.entities.size();

			// add new entity to pool
			pool.entities.push_back(Entity(entity_type_id, pool.entities.size(), _last_guid, pool.initial_update_mask ) );

			// increment guid source
			++_last_guid;

			return &pool.entities.back();
		}

		void deleteEntity(Entity* entity)
		{
			assert(entity);
			assert(entity->_type_id < _entity_pools.size());
			assert(entity->_local_id < _entity_pools[entity->_type_id].entities.size() );

			// get pool
			EntityPool& pool = _entity_pools[entity->_type_id];
			unsigned int deleted_entity_id = entity->_local_id;
			unsigned int last_entity_id = pool.entities.size()-1;

			// destroy entities' components
			for (unsigned int comp_family_id=0; comp_family_id<pool.components_mask.size(); ++comp_family_id)
			{
				// for each component contained for this entity type
				if (pool.components_mask[comp_family_id])
				{
					uint8_t* deleted_comp_ptr =
							pool.components_pool.getCompsData(comp_family_id)[deleted_entity_id*_comps_reg.componentSize(comp_family_id)];

					// explicitely call destructors for contained components of deleted entity
					_comps_reg.componentDestructor(comp_family_id)(deleted_comp_ptr);
				}
			}

			uint32_t removed_guid = pool.entities[last_entity_id]._guid;
			if (deleted_entity_id != last_entity_id )
			// deleting non-last entity
			{
				// move last entity to deleted entity
				pool.entities[deleted_entity_id]._guid = removed_guid;		// guid
				pool.entities[deleted_entity_id]._update_mask = pool.entities[last_entity_id]._update_mask;			// update mask

				// move components of last entity to deleted entity components
				for (unsigned int comp_family_id=0; comp_family_id<pool.components_mask.size(); ++comp_family_id)
				{
					if (pool.components_mask[comp_family_id])
					{
						unsigned int comp_size = _comps_reg.componentSize(comp_family_id);
						uint8_t* deleted_comp_ptr =
								pool.components_pool.getCompsData(comp_family_id)[deleted_entity_id*comp_size];
						uint8_t* last_comp_ptr =
								pool.components_pool.getCompsData(comp_family_id)[last_entity_id*comp_size];
						memcpy(deleted_comp_ptr, last_comp_ptr, comp_size);
					}
				}

				// change position mapping for last entity (which got moved to deleted entity place)
				EntityPos& last_ent_pos = _entities_pos_map[removed_guid];
				last_ent_pos.local_id = deleted_entity_id;
			}

			// remove position mapping for removed entity's guid
			_entities_pos_map.erase(removed_guid);
			// pop last entity
			pool.entities.pop_back();
		}


		// get temporary ptr for entity (can change when deleting entities)
		//	 returns NULL if entity does not exist
		Entity* getEntity(unsigned int guid)
		{
			std::map<uint32_t, uint32_t>::iterator it = _entities_pos_map.find(guid);
			if (it==_entities_pos_map.end())
				return NULL;
			// else
			EntityPos& pos = it->second;
			return &_entity_pools[pos.pool_id].entities[pos.local_id];
		}

		// get entity's component
		uint8_t* getComponent(Entity* entity, unsigned int component_family_id)
		{
			assert(entity);
			assert(entity->_type_id < _entity_pools.size());
			assert(entity->_local_id < _entity_pools[entity->_type_id].entities.size() );
			assert(_entity_pools[entity->_type_id].components_mask[component_family_id]
			       && "Entity type does not contain this component.");

			// get vector with components of desired type
			std::vector<uint8_t>& components_data = _entity_pools[entity->_type_id].components_pool.getCompsData(component_family_id);
			// get component pointer for desired entity
			return &components_data[entity->_local_id*_comps_reg.componentSize(component_family_id)];
		}

		template <typename CompType>
		CompType* getComponent(Entity* entity)
		{
			assert( CompType::familyId() != -1
					&& "Component must be registered");
			return (CompType*)getComponent(entity, CompType::familyId());
		}

	private:
		ComponentsRegister& _comps_reg;
		// for each entity type there is one pool
		std::vector<EntityPool> _entity_pools;

		// mapping from global entity id to EntityPos
		std::map<uint32_t, EntityPos> _entities_pos_map;

		uint32_t _last_guid;
	};
}



#endif /* ENTITIES_H_ */
