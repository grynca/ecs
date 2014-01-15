/*
 * Entity.cpp
 *
 *  Created on: 22. 12. 2013
 *      Author: lutze
 */

#include "ECS/Entity.h"

uint32_t Grynca::Entity::guid() const
{
	return _guid;
}

uint32_t Grynca::Entity::typeId() const
{
	return _type_id;
}

bool Grynca::Entity::isUpdatedBySystem(unsigned int system_type_id)
{
	assert(system_type_id<MAX_SYSTEMS);
	return _update_mask[system_type_id];
}

void Grynca::Entity::updateBySystem(unsigned int system_type_id, bool update)
{
	assert(system_type_id<MAX_SYSTEMS);
	_update_mask[system_type_id] = update;
}

const Grynca::SystemsMask& Grynca::Entity::updateMask()
{
	return _update_mask;
}

Grynca::Entity::Entity(uint32_t type_id, uint32_t local_id, uint32_t guid, SystemsMask& initial_update_mask)
 : _type_id(type_id), _local_id(local_id), _guid(guid),
   _update_mask(initial_update_mask)
{
}
