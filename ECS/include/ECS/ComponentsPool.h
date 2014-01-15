/*
 * ComponentsPool.h
 *
 *  Created on: 7. 1. 2014
 *      Author: lutze
 */

#ifndef COMPONENTSPOOL_H_
#define COMPONENTSPOOL_H_

#include <vector>
#include <cassert>
#include "ComponentsRegister.h"
#include "CommonDefs.h"

namespace Grynca
{
	class ComponentsPool
	{
	public:
		void init(ComponentsRegister& reg, ComponentsMask mask, unsigned int initial_capacity)
		{
			assert(initial_capacity > 0);

			// in case of reinit
			_components.clear();

			_capacity = initial_capacity;

			for (unsigned int comp_family_id=0; comp_family_id<mask.size(); ++comp_family_id)
			{
				if (mask[comp_family_id])
				// component should be contained in pool
				{
					// resize vector of vectors
					_components.resize(comp_family_id + 1);
					// resize inner vector so it can hold initial capacity of components
					_components.back().resize(_capacity * reg.componentSize(comp_family_id) );
				}
			}

		}

		// get data for components
		std::vector<uint8_t>& getCompsData(unsigned int component_family_id)
		{
			assert(component_family_id < _components.size() );

			return _components[component_family_id];
		}

		void doubleCapacity()
		{
			_capacity *=2;
			for (unsigned int comp_family_id=0; comp_family_id<_components.size(); ++comp_family_id)
			{
				if (_components[comp_family_id].size() > 0)		// if this component type is contained
					// double capacity of components array
					_components[comp_family_id].resize(_components[comp_family_id].size()*2);
			}
		}

		// get capacity - in number of entities
		unsigned int capacity() { return _capacity; }
	protected:


		// components for entities
		//	_components[comp_family_id][local_entity_id*sizeof(comp)]
		//  for not contained components inner vector will be zero sized
		std::vector<std::vector<uint8_t> > _components;
		unsigned int _capacity;
	};
}


#endif /* COMPONENTSPOOL_H_ */
