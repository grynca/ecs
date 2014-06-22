/*
 * Entity.h
 *
 *  Created on: 19. 12. 2013
 *      Author: lutza
 */

#ifndef GRYNCAENTITY_H_
#define GRYNCAENTITY_H_

#include "Components.h"

namespace Grynca {
    // fw
    class SystemBase;
    class EntityTypeInfo;

    class EntityBase {
    public:
        // get component data by component_type_id
        void* get(unsigned int component_type_id);

        template<typename ComponentType>
        ComponentType* get();

        bool canBeUpdatedBySystem(SystemBase* system);
        bool isUpdatedBySystem(SystemBase* system);
        void setUpdatingBySystem(SystemBase* system, bool set);

        uint32_t guid();
        SystemsMaskBits updateMask();

        EntityTypePool& getPool() { return *pool_; }
    protected:
        EntityBase() {}
        virtual ~EntityBase() {}

    private:
        friend class ECSManager;
        template<typename Derived, typename... Comps> friend class System;

        EntityTypePool* pool_;
        unsigned int chunk_id_;
        unsigned int chunk_pos_;
        void setGuid_(uint32_t guid);

    };

    template <typename Derived>
    class Entity : public EntityBase {
    public:
        static const EntityTypeInfo& getTypeInfo() {
            return EntitiesRegister::get(Derived::typeId);
        }

        // calls entity's destructor
        static void destroy(void* ent) {
            ((Derived*)ent)->~Derived();
        }

        // default no-op staticinit, can be shadowed in Derived class
        static void staticInit(ECSManager& /*manager*/, StaticComponentsPool& /*pool*/) {}
    };
}
#include "EntityTypePool.h"
#include "EntitiesRegister.h"
#include "EntityHeaderComponent.h"
#include "System.h"

namespace Grynca
{
    // get component data by component_type_id
    inline void* EntityBase::get(unsigned int component_type_id) {
        if (pool_->staticComponents().contains(component_type_id))
            return pool_->staticComponents().get(component_type_id);
        else if (pool_->dynamicComponents().contains(component_type_id))
            return pool_->dynamicComponents().get(chunk_id_, chunk_pos_, component_type_id);
        else
            return NULL;
    }

    template<typename ComponentType>
    inline ComponentType* EntityBase::get() {
        return (ComponentType*)get(ComponentType::typeId);
    }


    inline bool EntityBase::canBeUpdatedBySystem(SystemBase* system) {
        return (pool_->getTypeInfo().allComponentsMask() & system->neededComponentsMask()) == system->neededComponentsMask();
    }

    inline bool EntityBase::isUpdatedBySystem(SystemBase* system) {
        return get<EntityHeaderComponent>()->update_mask[system->systemId()];
    }

    inline void EntityBase::setUpdatingBySystem(SystemBase* system, bool set) {
        get<EntityHeaderComponent>()->update_mask[system->systemId()] = set;
    }

    inline uint32_t EntityBase::guid() {
        return get<EntityHeaderComponent>()->guid;
    }

    inline SystemsMaskBits EntityBase::updateMask() {
        return get<EntityHeaderComponent>()->update_mask;
    }

    inline void EntityBase::setGuid_(uint32_t guid) {
        get<EntityHeaderComponent>()->guid = guid;
    }

}

#endif /* GRYNCAENTITY_H_ */
