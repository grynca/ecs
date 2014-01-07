/*
 * EntitiesPool.h
 *
 *  Created on: 7. 1. 2014
 *      Author: lutze
 */

#ifndef ENTITIESPOOL_H_
#define ENTITIESPOOL_H_

#include <cassert>


class EntitiesPool
{
public:
	const unsigned int POOL_DEFAULT_CAPACITY = 128;

	template <typename ... Comps>
	void create(unsigned int initial_capacity = POOL_DEFAULT_CAPACITY)
	{
		assert(initial_capacity > 0);
		_capacity = initial_capacity;
		_createComponentsPool<Comps>();
	}



protected:
	void _doubleCapacity()
	{
		_capacity *=2;
		for (unsigned int i=0; i<_components.size(); i++)
		{
			if (_components[i].size() > 0)
				_components[i].resize(_components[i].size()*2);
		}
	}


	// recursion general case
	template<typename Comp, typename ... Comps>
	void _createComponentsPool()
	{
		_createComponentsPool<Comp>();
		_createComponentsPool<Comps>();
	}

	// recursion bottom
	template <typename Comp>
	void _createComponentsPool()
	{
		assert(Comp::familyId() != -1
				&& "Component is not registered.");
		if (Comp::familyId() >= _components.size() )
		{
			// resize outer vector
			_components.resize(Comp::familyId() + 1);
		}

		// resize inner vector to fit data for components
		_components[Comp::familyId()].resize( sizeof(Comp)*_capacity );
	}


	// components for entities
	//	_components[comp_family_id][local_entity_id*sizeof(comp)]
	std::vector<std::vector<uint8_t> > _components;
	unsigned int _capacity;
};



#endif /* ENTITIESPOOL_H_ */
