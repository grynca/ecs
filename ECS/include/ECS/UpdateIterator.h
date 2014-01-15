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

namespace Grynca
{
	// fw decl
	class Components;
	class Entity;

	// iterates over entities updated by system and their components data
	class UpdateIterator
	{
	public:
		UpdateIterator(ComponentsMask& relevant_components);

		// initialize iterator to iterate over specific pool (does not check system compatibility with pool)
		void setPool(Entities::EntityPool& pool);

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
