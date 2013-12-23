/*
 * SystemIterator.cpp
 *
 *  Created on: 22. 12. 2013
 *      Author: lutze
 */

#include "ECS/SystemIterator.h"
#include "ECS/Components.h"
#include "ECS/Entity.h"

Grynca::SystemIterator::SystemIterator()
 : _entities(NULL),
   _system_id(-1),
   _entity_local_id(-1)
{
}

void Grynca::SystemIterator::create(std::vector<Entity>& entities,
									unsigned int system_id,
									Components& components,
									const ComponentsMask& system_comps)
{
	_entities = &entities;
	_system_id = system_id;
	_all_comps.resize(MAX_COMPONENTS, -1);
	for (unsigned int comp_type_id=0; comp_type_id<system_comps.size(); comp_type_id++)
	{
		if (system_comps[comp_type_id])
		{
			_all_comps[comp_type_id] = _relevant_comps.size();
			// remember info about relevant components
			unsigned int comp_size = components.componentSize(comp_type_id);
			std::vector<uint8_t>& comps_data = components.getData(comp_type_id);
			_relevant_comps.push_back({comp_size, &comps_data});
		}
	}
}

void Grynca::SystemIterator::reset()
{
	assert(_system_id != -1);

	_entity_local_id = -1;
}

bool Grynca::SystemIterator::next()
{
	assert(_system_id != -1);

	while (_entity_local_id<(int)(_entities->size()-1) )
	{
		//skips entities that are not to be updated by system
		++_entity_local_id;
		Entity& e = getEntity();
		if (e.isUpdatedBySystem(_system_id))
			return true;
		// else skip this entity and loop to next
	}
	// iterated to the end of entities
	return false;
}

Grynca::Entity& Grynca::SystemIterator::getEntity()
{
	return (*_entities)[_entity_local_id];
}
