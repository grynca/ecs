/*
 * Systems.h
 *
 *  Created on: 8. 1. 2014
 *      Author: lutza
 */

#ifndef SYSTEMS_H_
#define SYSTEMS_H_

#include <vector>
#include <cassert>
#include "System.h"

namespace Grynca
{
	class Systems
	{
	public:
		~Systems();

		// use:
		//	registerSystem(new MovementSystem(...))
		template <typename Sys>
		void registerSystem(Sys* sys);

		BaseSystem* get(unsigned int system_id);

		template <typename SysType>
		SysType& get();

		unsigned int registeredSystemsCount();
	private:

		std::vector<BaseSystem*> _systems;
	};
}


inline Grynca::Systems::~Systems()
{
	// delete registered systems
	for (unsigned int sys_id=0; sys_id < _systems.size(); sys_id++ )
	{
		delete _systems[sys_id];
	}
}

template <typename Sys>
inline void Grynca::Systems::registerSystem(Sys* sys)
{
	assert(Sys::systemTypeId() == -1
			&& "System is already registered.");
	assert(_systems.size() < MAX_SYSTEMS
			&& "You have too many systems, change MAX_SYSTEMS if necessary.");

	Sys::_systemTypeId() = _systems.size();
	_systems.push_back(sys);
}


inline Grynca::BaseSystem* Grynca::Systems::get(unsigned int system_id)
{
	assert(system_id < _systems.size());
	return _systems[system_id];
}

template <typename SysType>
inline SysType& Grynca::Systems::get()
{
	assert(SysType::systemTypeId() != -1
			&& "System is not registered.");
	return *(SysType*)get(SysType::systemTypeId());
}

inline unsigned int Grynca::Systems::registeredSystemsCount()
{
	return _systems.size();
}


#endif /* SYSTEMS_H_ */
