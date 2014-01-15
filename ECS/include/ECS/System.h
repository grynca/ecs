/*
 * System.h
 *
 *  Created on: 21. 12. 2013
 *      Author: lutze
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <cassert>
#include "ComponentsMaskCreator.h"
#include "SystemIterator.h"
#include "ECSManager.h"

namespace Grynca
{
	class BaseSystem
	{
		friend class ECSManager;
	public:
		BaseSystem()
		 : _entity_manager(NULL)
		{}

		virtual ~BaseSystem() {}
		virtual void update(float dt) = 0;
		EntityManager& entityManager();
	protected:
		EntityManager* _entity_manager;		// will be set upon registration by ECSManager
	};


	/// Derived class must define updateImpl()
	template <typename Derived, typename... Comps>
	class System : public BaseSystem
	{
		friend class Systems;
	public:
		void update(float dt) override;


		static int systemTypeId();

		// to check if components mask contains components needed by this system:
		//		SysType::neededComponentsMask() & component_mask == SysType::neededComponentsMask()
		static const ComponentsMask& neededComponentsMask();
	private:

		static int& _systemTypeId();

		UpdateIterator _update_it;
	};

}

inline Grynca::EntityManager& Grynca::BaseSystem::entityManager()
{
	assert(_entity_manager);
	return *_entity_manager;
}

template <typename Derived, typename ... Comps>
inline void Grynca::System<Derived, Comps...>::updateEntities(Entities& entities, float dt)
{
	assert(systemTypeId() != -1
			&& "System must be registered.");

	for (unsigned int pool_id=0; pool_id < entities._entity_pools.size(); ++pool_id)
	{
		if ((neededComponentsMask() & entities._entity_pools[pool_id].components_mask) == neededComponentsMask())
			// pool contains components this system needs
		{
			_update_it.setPool(entities._entity_pools[pool_id]);
			// call derived class's update
			((Derived*)this)->updateImpl(dt, _update_it);
		}
	}
}

/*template <typename Derived, typename ... Comps>
inline void Grynca::System<Derived, Comps...>::_registerSystem(unsigned int s_id, EntityManager& em)
{
	assert(s_id < MAX_SYSTEMS
			&& "You have too many systems, change MAX_SYSTEMS if necessary.");
	// set static system id
	_systemId() = s_id;
	_entity_manager = &em;
	// create system iterator
	_system_it.create(_entity_manager->_entities, s_id,
					 _entity_manager->_components, neededComponentsMask());
}*/

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
	static unsigned int _system_type_id = -1;		// -1 means not registered
	return _system_type_id;
}

#endif /* SYSTEM_H_ */
