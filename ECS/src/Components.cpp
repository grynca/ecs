/*
 * Components.cpp
 *
 *  Created on: 22. 12. 2013
 *      Author: lutze
 */

#include "ECS/Components.h"

Grynca::Components::Components(unsigned int initial_capacity)
 : _capacity(initial_capacity)
{
	assert(initial_capacity > 0);
}

std::vector<uint8_t>& Grynca::Components::getAllData(unsigned int component_family_id)
{
	return _data[component_family_id];
}

uint8_t* Grynca::Components::getData(unsigned int component_family_id, unsigned int entity_id)
{
	assert((int)component_family_id != -1
			&& "Component is not registered.");
	assert(component_family_id < _data.size());
	unsigned int data_start = entity_id*_comps_sizes[component_family_id];
	assert(data_start < _data[component_family_id].size());

	return &_data[component_family_id][data_start];
}

unsigned int Grynca::Components::componentSize(unsigned int component_family_id)
{
	assert(component_family_id < _comps_sizes.size());
	return _comps_sizes[component_family_id];
}

void Grynca::Components::_doubleCapacity()
{
	_capacity *=2;
	for (unsigned int i=0; i<_data.size(); i++)
	{
		_data[i].resize(_comps_sizes[i]*_capacity);
	}
}

unsigned int Grynca::Components::registeredComponentsCount()
{
	return _data.size();
}

