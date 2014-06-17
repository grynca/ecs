/*
 * ECSManager.h
 *
 *  Created on: 19. 12. 2013
 *      Author: lutza
 */

#ifndef ECSMANAGER_H_
#define ECSMANAGER_H_

#include <map>
#include <stdint.h>
#include <vector>

namespace Grynca
{
    class EntitiesPool;
    class EntityBase;
    class SystemBase;

	class ECSManager
	{
	public:
        ECSManager();
        virtual ~ECSManager();

        template <typename EntityType, typename ... ConstructionArgs>
        EntityType* createEntity(ConstructionArgs... args);

        void destroyEntity(EntityBase *entity);

        EntityBase* getEntity(unsigned int guid);

        template <typename SystemType, typename ... ConstructionArgs>
        SystemType* createSystem(ConstructionArgs... args);

		// updates entities with all systems (in order they were registered)
        void updateAll(double dt);
	protected:

        void entityMoved_(uint32_t guid, EntityBase* ent);

        uint32_t guid_source_;
        std::map<uint32_t, EntityBase*> guid_to_entity_map_;
        std::vector<EntitiesPool*> pools_;
        std::vector<SystemBase*> systems_;
	};

}


#include "EntitiesRegister.h"
#include "EntitiesPool.h"
#include "System.h"

namespace Grynca {
    inline ECSManager::ECSManager()
     : guid_source_(0)
    {
        for (unsigned int i=0; i<EntitiesRegister::registeredTypesCount(); i++) {
            const EntityTypeInfo& eti = EntitiesRegister::getEntityTypeInfo(i);
            pools_.push_back(NULL);
            if (eti.isRegistered()) {
                pools_.back() = new EntitiesPool(&eti);
                pools_.back()->setEntityMovedCallback( std::bind(&ECSManager::entityMoved_, this, std::placeholders::_1, std::placeholders::_2) );
            }
        }
    }

    inline ECSManager::~ECSManager()
    {
        for (unsigned int i=0; i<pools_.size(); i++) {
            delete pools_[i];
        }
        for (unsigned int i=0; i<systems_.size(); i++) {
            delete systems_[i];
        }
    }

    template <typename EntityType, typename ... ConstructionArgs>
    inline EntityType* ECSManager::createEntity(ConstructionArgs... args)
    {
        uint32_t newent_guid = guid_source_;
        ++guid_source_;
        EntitiesPool* p = pools_[EntityType::typeId];
        EntityType* newent = p->createNewEntity<EntityType>(newent_guid, args...);
        guid_to_entity_map_[newent_guid] = newent;
        return newent;
    }

    inline void ECSManager::destroyEntity(EntityBase* entity)
    {
        guid_to_entity_map_.erase(entity->get<EntityHeaderComponent>()->guid);
        pools_[entity->getTypeInfo().type_id]->destroyEntity(entity);
    }

    inline EntityBase *ECSManager::getEntity(unsigned int guid) {
        auto it = guid_to_entity_map_.find(guid);
        if (it != guid_to_entity_map_.end())
            return it->second;
        return NULL;
    }

    template <typename SystemType, typename ... ConstructionArgs>
    inline SystemType* ECSManager::createSystem(ConstructionArgs... args) {
        SystemType* new_system = new SystemType(args...);
        new_system->system_id_ = (unsigned int)systems_.size();
        systems_.push_back(new_system);
        return new_system;
    }

    // updates entities with all systems (in order they were registered)
    inline void ECSManager::updateAll(double dt) {
        for (unsigned int i=0; i< systems_.size(); i++) {
            for (unsigned int j=0; j< pools_.size(); j++) {
                if (pools_[j] && systems_[i]->isPoolCompatible(*pools_[j]))
                    systems_[i]->updatePool(dt, *pools_[j]);
            }
        }
    }

    inline void ECSManager::entityMoved_(uint32_t guid, EntityBase* ent)
    {
        guid_to_entity_map_[guid] = ent;
    }
}

#endif /* ECSMANAGER_H_ */
