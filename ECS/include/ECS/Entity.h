/*
 * Entity.h
 *
 *  Created on: 19. 12. 2013
 *      Author: lutza
 */

#ifndef ENTITY_H_
#define ENTITY_H_

#include "EcsConfig.h"
#include "CommonDefs.h"
#include <cassert>

namespace Grynca
{
	class Entity
	{
		friend class EntityManager;
	public:
		// index to local entity manager (will change in time (when deleting entities from manager) )
		uint32_t localIndex() const;
		// globally unique id of entity, only persistent identificator of entity
		uint32_t guid() const;

		// TODO: porovnavani ==, !=, mozna dalsi (na zaklade guid)

		// TODO: isUpdatedBy<SystemType>(), canBeUpdatedBy<SystemType>() (pres SystemMask)

		bool containsComponent(unsigned int component_family_id);

		template <typename CompType>
		bool containsComponent();

		bool isUpdatedBySystem(unsigned int system_type_id);

		template <typename SysType>
		bool isUpdatedBySystem();

		// set if entity should be upadted by system or not
		void updateBySystem(unsigned int system_type_id, bool update);
		template <typename SysType>
		void updateBySystem(bool update);


		template <typename SysType>
		bool canBeUpdatedBySystem();


		const ComponentsMask& getComponentsMask();
		const SystemsMask& getSystemsMask();
	private:
		// created by EntityManager
		Entity(uint32_t local_id, uint32_t guid);

		// mask of valid components
		ComponentsMask _components_mask;
		// mask of systems that update entity
		SystemsMask	_systems_mask;
		uint32_t _local_id;
		uint32_t _guid;
	};
}


template <typename CompType>
inline bool Grynca::Entity::containsComponent()
{
	return containsComponent(CompType::familyId());
}

template <typename SysType>
inline bool Grynca::Entity::isUpdatedBySystem()
{
	return isUpdatedBySystem(SysType::systemId());
}

template <typename SysType>
void Grynca::Entity::updateBySystem(bool update)
{
	assert(canBeUpdatedBySystem<SysType>()
			&& "Entity can't be updated by this system (is missing some components).");
	updateBySystem(SysType::systemId(), update);
}

template <typename SysType>
inline bool Grynca::Entity::canBeUpdatedBySystem()
{
	return ((_components_mask & SysType::neededComponentsMask())
				== SysType::neededComponentsMask());
}

#endif /* ENTITY_H_ */
