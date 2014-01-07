/*
 * Systems.h
 *
 *  Created on: 19. 12. 2013
 *      Author: lutza
 */

#ifndef SYSTEMS_H_
#define SYSTEMS_H_

#include <vector>

namespace Grynca
{
	// fw decl
	class BaseSystem;
	class EntityManager;

	class Systems
	{
		friend class EntityManager;
	public:
		Systems(EntityManager& em)
	     : _entity_manager(em)
		{
		}

		~Systems();

		BaseSystem* get(unsigned int system_id);

		template <typename SysType>
		SysType* get();

		template <typename SysType>
		void registerSystem(SysType* system);

		unsigned int systemsCount();
	private:
		EntityManager& _entity_manager;
		std::vector<BaseSystem*> _systems;
	};
}

template <typename SysType>
inline SysType* Grynca::Systems::get()
{
	assert(SysType::systemId() != -1
			&& "System is not registered.");
	return get(SysType::systemId());
}

template <typename SysType>
inline void Grynca::Systems::registerSystem(SysType* system)
{
	// register system to entity manager
	system->_registerSystem(_systems.size(), _entity_manager);
	_systems.push_back(system);
}

#endif /* SYSTEMS_H_ */
