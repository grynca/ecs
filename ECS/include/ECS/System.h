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
#include "EntityManager.h"

namespace Grynca
{
	class BaseSystem
	{
	public:
		BaseSystem()
		 : _entity_manager(NULL)
		{}

		virtual ~BaseSystem() {}
		virtual void update(float dt) = 0;
		EntityManager& entityManager();
	protected:
		EntityManager* _entity_manager;
	};


	template <typename Derived, typename... Comps>
	class System : public BaseSystem
	{
		friend class Systems;
	public:
		void update(float dt) override;

		static unsigned int systemId();

		static const ComponentsMask& neededComponentsMask();
	private:
		// register system to entity manager
		void _registerSystem(unsigned int s_id, EntityManager& em);

		static unsigned int& _systemId();

		SystemIterator _system_it;
	};

}

inline Grynca::EntityManager& Grynca::BaseSystem::entityManager()
{
	assert(_entity_manager);
	return *_entity_manager;
}

template <typename Derived, typename ... Comps>
inline void Grynca::System<Derived, Comps...>::update(float dt)
{
	assert(systemId() != -1
			&& "System must be registered.");
	// reset iterator
	_system_it.reset();
	// call derived class's update
	((Derived*)this)->updateImpl(dt, _system_it);
}

template <typename Derived, typename ... Comps>
inline unsigned int Grynca::System<Derived, Comps...>::systemId()
//static
{
	return _systemId();
}

template <typename Derived, typename ... Comps>
inline const Grynca::ComponentsMask& Grynca::System<Derived, Comps...>::neededComponentsMask()
//static
{
	static ComponentsMask needed_comps_mask = ComponentsMaskCreator<Comps...>().mask;
	return needed_comps_mask;
}

template <typename Derived, typename ... Comps>
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
}

template <typename Derived, typename ... Comps>
inline unsigned int& Grynca::System<Derived, Comps...>::_systemId()
//static
{
	static unsigned int _system_id = -1;		// -1 means not registered
	return _system_id;
}


#endif /* SYSTEM_H_ */
