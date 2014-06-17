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

        // only one instance of systemtype can be created
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
            EntityTypeInfo& eti = EntitiesRegister::getEntityTypeInfo(i);
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
        EntitiesPool* p = pools_[EntityType::entityTypeId];
        EntityType* newent = p->createNewEntity<EntityType>(newent_guid, args...);
        guid_to_entity_map_[newent_guid] = newent;
        return newent;
    }

    inline void ECSManager::destroyEntity(EntityBase* entity)
    {
        guid_to_entity_map_.erase(entity->get<EntityHeaderComponent>()->guid);
        pools_[entity->getTypeInfo().getTypeId()]->destroyEntity(entity);
    }

    inline EntityBase *ECSManager::getEntity(unsigned int guid) {
        auto it = guid_to_entity_map_.find(guid);
        if (it != guid_to_entity_map_.end())
            return it->second;
        return NULL;
    }

    template <typename SystemType, typename ... ConstructionArgs>
    inline SystemType* ECSManager::createSystem(ConstructionArgs... args) {
        if (SystemType::systemTypeId >= systems_.size())
            systems_.resize(SystemType::systemTypeId+1, NULL);
        assert(!systems_[SystemType::systemTypeId] && "System of this type already present in this ECSManager.");
        systems_.back() = new SystemType(args...);
        systems_.back()->system_id_ = SystemType::systemTypeId;
        return (SystemType*)systems_.back();
    }

    // updates entities with all systems (in order they were registered)
    inline void ECSManager::updateAll(double dt) {
        for (unsigned int i=0; i< systems_.size(); i++) {
            if (!systems_[i])
                continue;
            for (unsigned int j=0; j< pools_.size(); j++) {
                if (pools_[j] &&  pools_[j]->getEntityTypeInfo().relevantSystems()[i]) {
                    assert(systems_[i]->isPoolCompatible(*pools_[j]) && "System is not compatible with entity type.");
                    systems_[i]->updatePool(dt, *pools_[j]);
                }
            }
        }
    }

    inline void ECSManager::entityMoved_(uint32_t guid, EntityBase* ent)
    {
        guid_to_entity_map_[guid] = ent;
    }
}

#endif /* ECSMANAGER_H_ */
