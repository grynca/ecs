/*
 * ECSManager.h
 *
 *  Created on: 19. 12. 2013
 *      Author: lutza
 */

#ifndef GRYNCAECSMANAGER_H_
#define GRYNCAECSMANAGER_H_

#include <map>
#include <stdint.h>
#include <vector>

namespace Grynca
{
    class EntityTypePool;
    class EntityBase;
    class SystemBase;

	class ECSManager
	{
	public:
        ECSManager();
        virtual ~ECSManager();

        // calls staticInit() for entity types
        void ECSManager::doStaticInit();

        template <typename EntityType, typename ... ConstructionArgs>
        EntityType* createEntity(EntityType* newent, ConstructionArgs... args);

        void destroyEntity(EntityBase *entity);

        EntityBase* getEntity(unsigned int guid);

        // only one instance of systemtype can be created
        template <typename SystemType, typename ... ConstructionArgs>
        SystemType* createSystem(ConstructionArgs... args);

        SystemBase* getSystem(unsigned int system_type_id);
        template <typename SystemType>
        SystemType* getSystem();

		// updates entities with all systems (in order they were registered)
        void updateAll(double dt);
	protected:

        void entityMoved_(uint32_t guid, unsigned int new_pos);

        uint32_t guid_source_;
        std::map<uint32_t, EntityBase*> guid_to_entity_map_;
        std::vector<EntityTypePool*> pools_;
        std::vector<SystemBase*> systems_;
	};

}


#include "EntitiesRegister.h"
#include "EntityTypePool.h"
#include "EntityHeaderComponent.h"
#include "System.h"

namespace Grynca {
    inline ECSManager::ECSManager()
     : guid_source_(0)
    {
        for (unsigned int i=0; i<EntitiesRegister::registeredTypesCount(); i++) {
            EntityTypeInfo& eti = EntitiesRegister::get(i);
            pools_.push_back(NULL);
            if (eti.isRegistered()) {
                pools_.back() = new EntityTypePool(eti, *this);
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

    inline void ECSManager::doStaticInit() {
        for (unsigned int i=0; i<pools_.size(); ++i) {
            if (!pools_[i])
                continue;
            pools_[i]->getTypeInfo().staticInitFunc()(*this, pools_[i]->staticComponents());
        }
    }

    template <typename EntityType, typename ... ConstructionArgs>
    inline EntityType* ECSManager::createEntity(EntityType* newent, ConstructionArgs... args)
    {
        uint32_t newent_guid = guid_source_;
        ++guid_source_;
        EntityTypePool* p = pools_[EntityType::typeId];
        auto ent_pos = p->createNewEntity();

        newent->pool_ = p;
        newent->chunk_id_ = ent_pos.first;
        newent->chunk_pos_ = ent_pos.second;
        newent->setGuid_(newent_guid);
        guid_to_entity_map_[newent_guid] = newent;
        newent->create(args...);
        return newent;
    }

    inline void ECSManager::destroyEntity(EntityBase* entity)
    {
        // get instance managed with manager
        uint32_t guid = entity->guid();
        EntityBase* myent = guid_to_entity_map_[guid];
        // call its destructor
        myent->getPool().getTypeInfo().destroyFunc()(myent);
        // destroy entity in pool
        entity->getPool().destroyEntity(entity->chunk_id_, entity->chunk_pos_);
        // erase record from map
        guid_to_entity_map_.erase(guid);
    }

    inline EntityBase *ECSManager::getEntity(unsigned int guid) {
        auto it = guid_to_entity_map_.find(guid);
        if (it != guid_to_entity_map_.end())
            return it->second;
        return NULL;
    }

    template <typename SystemType, typename ... ConstructionArgs>
    inline SystemType* ECSManager::createSystem(ConstructionArgs... args) {
        if (SystemType::typeId >= systems_.size())
            systems_.resize(SystemType::typeId+1, NULL);
        assert(!systems_[SystemType::typeId] && "System of this type already present in this ECSManager.");
        systems_.back() = new SystemType(args...);
        systems_.back()->system_id_ = SystemType::typeId;
        return (SystemType*)systems_.back();
    }

    inline SystemBase* ECSManager::getSystem(unsigned int system_type_id) {
        assert(system_type_id<systems_.size()
               && systems_[system_type_id]
               && "System not registered in ECSManager.");
        return systems_[system_type_id];
    }

    template <typename SystemType>
    inline SystemType* ECSManager::getSystem() {
        return (SystemType*)getSystem(SystemType::typeId);
    }

    // updates entities with all systems (in order they were registered)
    inline void ECSManager::updateAll(double dt) {
        for (unsigned int i=0; i< systems_.size(); i++) {
            if (!systems_[i])
                continue;
            for (unsigned int j=0; j< pools_.size(); j++) {
                if (!pools_[j])
                    continue;
                if (!pools_[j]->staticComponents().get<EntityTypeHeaderComponent>()->relevant_systems[i])
                    // system not relevant for this entity type
                    continue;
                assert(systems_[i]->isPoolCompatible(*pools_[j]) && "System is not compatible with entity type.");
                systems_[i]->updatePool(dt, pools_[j]);
            }
        }
    }

    inline void ECSManager::entityMoved_(uint32_t guid, unsigned int new_pos)
    {
        guid_to_entity_map_[guid]->chunk_pos_ = new_pos;
    }
}

#endif /* GRYNCAECSMANAGER_H_ */
