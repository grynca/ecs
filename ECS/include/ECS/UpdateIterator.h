/*
 * UpdateIterator.h
 *
 *  Created on: 21. 12. 2013
 *      Author: lutze
 */

#ifndef UPDATEITERATOR_H_
#define UPDATEITERATOR_H_

#include <cassert>
#include <vector>
#include <stdint.h>
#include "CommonDefs.h"
#include "UpdateIterator.h"
#include "ComponentsPool.h"
#include "Entity.h"
#include "Entities.h"

namespace Grynca
{
	// iterates over entities updated by system and their components data
	class UpdateIterator
	{
	public:
		UpdateIterator(int system_id, const ComponentsMask& system_components);

		// initialize iterator to iterate over specific pool (does not check system compatibility with pool)
		void setPool(EntityPool& pool);

		bool next();

		Entity& getEntity();

		template <typename CompType>
		CompType& get();

	private:

		// for non contained components NULL
		//	for contained components pointer to comps vector currently being iterated
		std::vector<std::vector<uint8_t>*> _components;
		// ptr to entities currently being iterated
		std::vector<Entity>* _entities;

		int _entity_local_id;
		int _system_id;
	};
}

inline Grynca::UpdateIterator::UpdateIterator(int system_id, const ComponentsMask& system_components)
: _entities(NULL),
  _entity_local_id(-1),
  _system_id(system_id)
{
	_components.resize(system_components.size(), NULL);
	for (unsigned int i=0; i<system_components.size(); ++i)
	{
		if (system_components[i])
			_components[i] = (std::vector<uint8_t>*)1;		// set not NULL to relevant components
	}
}

inline void Grynca::UpdateIterator::setPool(EntityPool& pool)
{
	_entity_local_id = -1;
	// get ptr to pool entities
	_entities = &pool.entities;
	// get ptrs to pool components arrays
	for (unsigned int i=0; i<_components.size(); ++i)
	{
		if (_components[i])
			// component is relevant
			_components[i] = &pool.components_pool.getCompsData(i);
	}
}

inline bool Grynca::UpdateIterator::next()
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

inline Grynca::Entity& Grynca::UpdateIterator::getEntity()
{
	return (*_entities)[_entity_local_id];
}


template <typename CompType>
inline CompType& Grynca::UpdateIterator::get()
{
	assert(_entity_local_id != -1);
	assert(CompType::familyId()!=-1
			&& "Component is not registered.");
	assert(!_components[CompType::familyId()]!=-1
	       && "Iterator does not track this component.");

	unsigned int data_start = _entity_local_id*sizeof(CompType);
	return *(CompType*)&((*_components[CompType::familyId()])[data_start]);
}

#endif /* UPDATEITERATOR_H_ */
