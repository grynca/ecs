/*
 * Systems.cpp
 *
 *  Created on: 22. 12. 2013
 *      Author: lutze
 */

#include "ECS/Systems.h"
#include "ECS/System.h"

Grynca::Systems::~Systems()
{
	for (unsigned int i=0; i<_systems.size(); i++)
		delete _systems[i];
}

Grynca::BaseSystem* Grynca::Systems::get(unsigned int system_id)
{
	assert(system_id < _systems.size());
	return _systems[system_id];
}

unsigned int Grynca::Systems::systemsCount()
{
	return _systems.size();
}
