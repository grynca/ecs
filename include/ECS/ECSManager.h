/*
 * ECSManager.h
 *
 *  Created on: 19. 12. 2013
 *      Author: lutza
 */

#ifndef ECSMANAGER_H_
#define ECSMANAGER_H_

#include "ComponentsRegister.h"
#include "Entities.h"
#include "Systems.h"

namespace Grynca
{
	class ECSManager
	{
	public:
		// getters
		ComponentsRegister& componentsRegister();
		Entities& entities();
		Systems& systems();

		// updates entities with all systems (in order they were registered)
		void updateAll(float dt);
	protected:
		// created by derivative
		ECSManager();

		ComponentsRegister _components_register;
		Entities _entities;
		Systems _systems;
	};

}

inline Grynca::ComponentsRegister& Grynca::ECSManager::componentsRegister()
{
	return _components_register;
}
inline Grynca::Entities& Grynca::ECSManager::entities()
{
	return _entities;
}
inline Grynca::Systems& Grynca::ECSManager::systems()
{
	return _systems;
}

// updates entities with all systems (in order they were registered)
inline void Grynca::ECSManager::updateAll(float dt)
{
	for (unsigned int sys_id=0; sys_id<systems().registeredSystemsCount(); ++sys_id)
	{
		systems().get(sys_id)->updateEntities(entities(), dt);
	}
}

inline Grynca::ECSManager::ECSManager()
 : _entities(_components_register)
{
}


#endif /* ECSMANAGER_H_ */
