/*
 * Entity.h
 *
 *  Created on: 19. 12. 2013
 *      Author: lutza
 */

#ifndef ENTITY_H_
#define ENTITY_H_

#include "EcsConfig.h"
#include "CommonDefs.h"
#include <cassert>

namespace Grynca
{
	class Entity
	{
		friend class Entities;
	public:
		// globally unique id of entity, only persistent identificator of entity
		uint32_t guid() const;

		uint32_t typeId() const;

		// TODO: porovnavani ==, !=, mozna dalsi (na zaklade guid)

		bool isUpdatedBySystem(unsigned int system_type_id);

		template <typename SysType>
		bool isUpdatedBySystem();

		// set updating by system (must have needed components by system or else system will ignore it)
		void updateBySystem(unsigned int system_type_id, bool update);
		template <typename SysType>
		void updateBySystem(bool update);

		// get update mask
		const SystemsMask& updateMask();
	private:
		// created by Entities
		Entity(uint32_t type_id, uint32_t local_id, uint32_t guid, SystemsMask& initial_update_mask);

		uint32_t _type_id;
		uint32_t _local_id;	// local id in entities pool
		uint32_t _guid;
		// mask of systems that update entity
		SystemsMask	_update_mask;

	};
}

template <typename SysType>
inline bool Grynca::Entity::isUpdatedBySystem()
{
	return isUpdatedBySystem(SysType::systemTypeId());
}

template <typename SysType>
void Grynca::Entity::updateBySystem(bool update)
{
	updateBySystem(SysType::systemTypeId(), update);
}

inline uint32_t Grynca::Entity::guid() const
{
	return _guid;
}

inline uint32_t Grynca::Entity::typeId() const
{
	return _type_id;
}

inline bool Grynca::Entity::isUpdatedBySystem(unsigned int system_type_id)
{
	assert(system_type_id<MAX_SYSTEMS);
	return _update_mask[system_type_id];
}

inline void Grynca::Entity::updateBySystem(unsigned int system_type_id, bool update)
{
	assert(system_type_id<MAX_SYSTEMS);
	_update_mask[system_type_id] = update;
}

inline const Grynca::SystemsMask& Grynca::Entity::updateMask()
{
	return _update_mask;
}

inline Grynca::Entity::Entity(uint32_t type_id, uint32_t local_id, uint32_t guid, SystemsMask& initial_update_mask)
 : _type_id(type_id), _local_id(local_id), _guid(guid),
   _update_mask(initial_update_mask)
{
}


#endif /* ENTITY_H_ */
