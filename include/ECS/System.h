/*
 * System.h
 *
 *  Created on: 21. 12. 2013
 *      Author: lutze
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <cassert>
#include "MaskCreators.h"
#include "UpdateIterator.h"
#include "Entities.h"

namespace Grynca
{
	// fw
	class ECSManager;

	class BaseSystem
	{
		friend class ECSManager;
	public:

		virtual ~BaseSystem() {}

		// updates provided entities with this system
		virtual void updateEntities(Entities& entities, float dt) = 0;
	};


	/// Derived class must define updateImpl()
	template <typename Derived, typename... Comps>
	class System : public BaseSystem
	{
		friend class Systems;
	public:

		void updateEntities(Entities& entities, float dt) override;

		static int systemTypeId();

		// to check if components mask contains components needed by this system:
		//		SysType::neededComponentsMask() & component_mask == SysType::neededComponentsMask()
		static const ComponentsMask& neededComponentsMask();
	private:

		static int& _systemTypeId();
	};

}

template <typename Derived, typename ... Comps>
inline void Grynca::System<Derived, Comps...>::updateEntities(Entities& entities, float dt)
{
	assert(systemTypeId() != -1
			&& "System must be registered.");

	// create update iterator
	UpdateIterator update_it(systemTypeId(), neededComponentsMask());

	for (unsigned int pool_id=0; pool_id < entities._entity_pools.size(); ++pool_id)
	{
		if ((neededComponentsMask() & entities._entity_pools[pool_id].components_mask) == neededComponentsMask())
		// pool contains components this system needs
		{
			update_it.setPool(entities._entity_pools[pool_id]);
			// call derived class's update
			((Derived*)this)->updateImpl(dt, update_it);
		}
	}
}

template <typename Derived, typename ... Comps>
inline int Grynca::System<Derived, Comps...>::systemTypeId()
//static
{
	return _systemTypeId();
}

template <typename Derived, typename ... Comps>
inline const Grynca::ComponentsMask& Grynca::System<Derived, Comps...>::neededComponentsMask()
//static
{
	static ComponentsMask needed_comps_mask = ComponentsMaskCreator<Comps...>().mask;
	return needed_comps_mask;
}

template <typename Derived, typename ... Comps>
inline int& Grynca::System<Derived, Comps...>::_systemTypeId()
//static
{
	static int _system_type_id = -1;		// -1 means not registered
	return _system_type_id;
}

#endif /* SYSTEM_H_ */
