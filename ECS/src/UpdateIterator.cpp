/*
 * SystemIterator.cpp
 *
 *  Created on: 22. 12. 2013
 *      Author: lutze
 */

#include "ECS/UpdateIterator.h"
#include "ECS/Components.h"
#include "ECS/Entity.h"

Grynca::UpdateIterator::UpdateIterator(ComponentsMask& relevant_components)
: _entity_local_id(-1)
{
	_components.resize(relevant_components.size(), NULL);
	for (unsigned int i=0; i<relevant_components.size(); ++i)
	{
		if (relevant_components[i])
			_components[i] = 1;		// set not NULL to relevant components
	}
}


// initialize iterator to iterate over specific pool (does not check system compatibility with pool)
void Grynca::UpdateIterator::setPool(Entities::EntityPool& pool)
{
	_entity_local_id = -1;
	// get ptr to pool entities
	_entities = &pool.entities;
	// get ptrs to pool components arrays
	for (unsigned int i=0; i<_components.size(); ++i)
	{
		if (_components[i])
			// component is relevant
			_components[i] = pool.components_pool.getCompsData(i);
	}
}

bool Grynca::UpdateIterator::next()
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

Grynca::Entity& Grynca::UpdateIterator::getEntity()
{
	return (*_entities)[_entity_local_id];
}
