/*
 * Entity.h
 *
 *  Created on: 19. 12. 2013
 *      Author: lutza
 */

#ifndef ENTITY_H_
#define ENTITY_H_

#include "TypeTuple.h"
#include "EntityHeaderComponent.h"
#include "Masks.h"

namespace Grynca {
    // fw
    class SystemBase;
    struct EntityTypeInfo;

    class EntityBase {
    public:
        // get component data by component_type_id
        void* get(unsigned int component_type_id);

        template<typename ComponentType>
        ComponentType* get();

        bool canBeUpdatedBySystem(SystemBase* system);
        bool isUpdatedBySystem(SystemBase* system);
        void setUpdatingBySystem(SystemBase* system, bool set);

        const EntityTypeInfo& getTypeInfo();
    protected:
        EntityBase(unsigned int entity_type_id) : entity_type_id_(entity_type_id) {}

    private:
        unsigned int entity_type_id_;
        //updated_by_[system_id] == true if system updates this entity
        std::vector<bool> updated_by_;
    };

    template <typename Derived, typename ... Comps>
    class Entity : public EntityType<Derived>, public EntityBase {
    public:
        using ComponentsTuple = TypeTuple<Comps..., EntityHeaderComponent>;
//        using EntityBase::get;      // otherwise it would be shadowed by templated get

        static ComponentsMask<Comps..., EntityHeaderComponent>& getComponentsMask() {
            static ComponentsMask<Comps..., EntityHeaderComponent> m;
            return m;
        }

//        template <typename ComponentType>
//        ComponentType& get() {
//            return type_tuple_.get<ComponentType>();
//        }

    protected:
        friend class EntitiesPool;

        Entity() : EntityBase(Derived::typeId) {}

        ComponentsTuple type_tuple_;
    };
}

#include "EntitiesRegister.h"
#include "EntityHeaderComponent.h"
#include "System.h"

namespace Grynca
{
    // get component data by component_type_id
    inline void* EntityBase::get(unsigned int component_type_id) {
        int offset = getTypeInfo().component_offsets[component_type_id];
        if (offset == -1)
            return NULL;
        else
            return ((uint8_t*)this)+sizeof(EntityBase)+offset;
    }

    template<typename ComponentType>
    inline ComponentType* EntityBase::get() {
        return (ComponentType*)get(ComponentType::typeId);
    }

    inline const EntityTypeInfo& EntityBase::getTypeInfo() {
        return EntitiesRegister::getEntityTypeInfo(entity_type_id_);
    }

    inline bool EntityBase::canBeUpdatedBySystem(SystemBase* system) {
        return (getTypeInfo().components_mask & system->neededComponentsMask()) == getTypeInfo().components_mask;
    }

    inline bool EntityBase::isUpdatedBySystem(SystemBase* system) {
        if (system->systemId() >= updated_by_.size())
            return false;
        return updated_by_[system->systemId()];
    }

    inline void EntityBase::setUpdatingBySystem(SystemBase* system, bool set) {
        if (system->systemId() >= updated_by_.size())
            updated_by_.resize(system->systemId()+1, false);
        updated_by_[system->systemId()] = set;
    }
}

#endif /* ENTITY_H_ */
