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

namespace Grynca
{
	class Systems
	{
	public:
		~Systems()
		{
			// delete registered systems
			for (unsigned int sys_id=0; sys_id < _systems.size(); sys_id++ )
			{
				delete _systems[sys_id];
			}
		}

		// use:
		//	registerSystem(new MovementSystem(...))
		template <typename Sys>
		void registerSystem(Sys* sys)
		{
			assert(Sys::systemTypeId() == -1
					&& "System is already registered.");
			assert(_systems.size() < MAX_SYSTEMS
					&& "You have too many systems, change MAX_SYSTEMS if necessary.");

			Sys::_systemTypeId() = _systems.size();
			_systems.push_back(sys);
		}


		BaseSystem* get(unsigned int system_id)
		{
			assert(system_id < _systems.size());
			return _systems[system_id];
		}

		template <typename SysType>
		SysType& get()
		{
			assert(SysType::systemTypeId() == -1
					&& "System is not registered.");
			return *(SysType*)get(SysType::systemTypeId());
		}

		unsigned int registeredSystemsCount()
		{
			return _systems.size();
		}
	private:

		std::vector<BaseSystem*> _systems;
	};
}



#endif /* SYSTEMS_H_ */
