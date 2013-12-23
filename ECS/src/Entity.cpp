/*
 * Entity.cpp
 *
 *  Created on: 22. 12. 2013
 *      Author: lutze
 */

#include "ECS/Entity.h"

uint32_t Grynca::Entity::localIndex() const
{
	return _local_id;
}

uint32_t Grynca::Entity::guid() const
{
	return _guid;
}


bool Grynca::Entity::containsComponent(unsigned int component_family_id)
{
	assert(component_family_id<MAX_COMPONENTS);
	return _components_mask[component_family_id];
}

bool Grynca::Entity::isUpdatedBySystem(unsigned int system_type_id)
{
	assert(system_type_id<MAX_SYSTEMS);
	return _systems_mask[system_type_id];
}

void Grynca::Entity::updateBySystem(unsigned int system_type_id, bool update)
{
	assert(system_type_id<MAX_SYSTEMS);
	_systems_mask[system_type_id] = update;
}

const Grynca::ComponentsMask& Grynca::Entity::getComponentsMask()
{
	return _components_mask;
}

const Grynca::SystemsMask& Grynca::Entity::getSystemsMask()
{
	return _systems_mask;
}

Grynca::Entity::Entity(uint32_t local_id, uint32_t guid)
 : _local_id(local_id), _guid(guid)
{
}
