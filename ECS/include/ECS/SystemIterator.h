/*
 * SystemIterator.h
 *
 *  Created on: 21. 12. 2013
 *      Author: lutze
 */

#ifndef SYSTEMITERATOR_H_
#define SYSTEMITERATOR_H_

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
	class SystemIterator
	{
	public:
		SystemIterator();
		// proper create
		void create(std::vector<Entity>& entities,
				    unsigned int system_id,
				    Components& components,
				    const ComponentsMask& system_comps);

		void reset();
		bool next();

		Entity& getEntity();

		template <typename CompType>
		CompType& get();

	private:
		struct ComponentInfo
		{
			unsigned int comp_size;
			std::vector<uint8_t>* comp_data;
		};
		// ids of relevant components
		std::vector<ComponentInfo> _relevant_comps;
		// for non relevant comps it is -1
		//	for relevant comps contains id to _relevant_comps vec
		std::vector<int> _all_comps;
		std::vector<Entity>* _entities;
		int _system_id;
		int _entity_local_id;
	};
}

template <typename CompType>
inline CompType& Grynca::SystemIterator::get()
{
	assert(_entity_local_id != -1);
	assert(CompType::familyId()!=-1
			&& "Component is not registered.");
	assert(_all_comps[CompType::familyId()]!=-1
	       && "Component is not relevant for this system.");
	ComponentInfo& ci = _relevant_comps[_all_comps[CompType::familyId()]];
	unsigned int data_start = _entity_local_id * ci.comp_size;
	return *(CompType*)&(*ci.comp_data)[data_start];
}

#endif /* SYSTEMITERATOR_H_ */
