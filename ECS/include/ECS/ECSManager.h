/*
 * ECSManager.h
 *
 *  Created on: 19. 12. 2013
 *      Author: lutza
 */

#ifndef ECSMANAGER_H_
#define ECSMANAGER_H_

#include <map>
#include <cassert>
#include "Components.h"
#include "Systems.h"
#include "Entity.h"

namespace Grynca
{
	class ECSManager
	{
	public:

		ComponentsRegister& componentsRegister() { return _components_register; }
		Entities& entities() { return _entities; }
		Systems& systems() { return _systems; }

	protected:
		/// Recursive helpers for derived classes
		//		for registering components, entities and systems

		template <typename C>
		void _registerComponent()
		{
			_components_register.registerComponent<C>();
		}

		template <typename E>
		void _registerEntityType(unsigned int initial_capacity)
		{
			_entities.registerEntityType<E>(initial_capacity);
		}

		template <typename S>
		void _registerSystem(S* s)
		{
			s->_entity_manager = this;
			_systems.registerSystem(s);
		}


		// created by derivative
		ECSManager()
		{
		}



		ComponentsRegister _components_register;
		Entities _entities;
		Systems _systems;
	};

}

#endif /* ECSMANAGER_H_ */
