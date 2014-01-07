/*
 * Components.h
 *
 *  Created on: 20. 12. 2013
 *      Author: lutza
 */

#ifndef COMPONENTS_H_
#define COMPONENTS_H_

#include <cassert>
#include <stdint.h>
#include <vector>

namespace Grynca
{
	class Components
	{
		friend class EntityManager;
		// component destructor func
		typedef void (*DestructorFunc)(uint8_t*);
	public:
		Components(unsigned int initial_capacity);

		// get data for all components of certain family_id
		//		( size will be larger than needed for entity comps, because of preallocation )
		std::vector<uint8_t>& getAllData(unsigned int component_family_id);
		// get single component data
		uint8_t* getData(unsigned int component_family_id, unsigned int entity_id);
		template <typename Comp>
		Comp* get(unsigned int entity_id);

		unsigned int componentSize(unsigned int component_family_id);

		template <typename Comp>
		void registerComponent();

		unsigned int registeredComponentsCount();
	protected:
		void _doubleCapacity();

		// _data[component_family][entity_id*sizeof(comp)]
		std::vector<unsigned int> _comps_sizes;
		std::vector<std::vector<uint8_t> > _data;
		// ptrs to destructors for components
		std::vector<DestructorFunc> _destructors;
		unsigned int _capacity;
	};

}

template <typename Comp>
inline Comp* Grynca::Components::get(unsigned int entity_id)
{
	return (Comp*)getData(Comp::familyId(), entity_id);
}

template <typename Comp>
inline void Grynca::Components::registerComponent()
{
	assert(Comp::familyId() == -1
			&& "Component is already registered.");
	Comp::_setFamilyId(_data.size());
	_data.resize(_data.size() +1);
	_data.back().resize( sizeof(Comp)*_capacity );
	_comps_sizes.push_back(sizeof(Comp));
	_destructors.push_back(Comp::_destructor);
}


#endif /* COMPONENTS_H_ */
