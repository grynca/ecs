/*
 * System.h
 *
 *  Created on: 21. 12. 2013
 *      Author: lutze
 */

#ifndef GRYNCASYSTEM_H_
#define GRYNCASYSTEM_H_

#include <cassert>
#include "CommonDefs.h"

namespace Grynca {
    // fw
    class EntityTypePool;
    class EntityBase;

    class SystemBase {
    public:
        bool isPoolCompatible(const EntityTypePool& pool);

        // pool must be compatible
        virtual void updatePool(double dt, EntityTypePool* pool) = 0;

        unsigned int systemId() { return system_id_; }
        const ComponentsMaskBits& neededComponentsMask() { return needed_components_; }
    protected:
        SystemBase() {}

        virtual ~SystemBase() {}

        // components needed by this system
        ComponentsMaskBits needed_components_;
    private:
        friend class ECSManager;
        // set during construction by ECSmanager
        unsigned int system_id_;
    };

    //// Derived class must define updateImpl()
    template <typename Derived, typename... Comps>
    class System : public SystemBase
    {
    public:
        System();

        virtual ~System() {}

        void updatePool(double dt, EntityTypePool* pool) override {
            EntityBase eb;
            eb.pool_ = pool;
            DynamicComponentsPool& dcp = pool->dynamicComponents();
            for (eb.chunk_id_ =0; eb.chunk_id_<dcp.chunksCount(); ++eb.chunk_id_)
            for (eb.chunk_pos_ =0; eb.chunk_pos_<dcp.entitiesInChunk(eb.chunk_id_); ++eb.chunk_pos_) {
                ((Derived*)this)->updateEntity(dt, eb);
            }
        }
    };

}

#include "Entity.h"
#include "TypeIdMask.h"

namespace Grynca {
    inline bool SystemBase::isPoolCompatible(const EntityTypePool &pool) {
        return (pool.getTypeInfo().allComponentsMask() & needed_components_) == needed_components_;
    }

    template <typename Derived, typename... Comps>
    inline System<Derived, Comps...>::System()
    {
        needed_components_ = TypeIdMask<MAX_SYSTEMS, Comps...>().bits;   // construct needed components mask
    }

}


#endif /* GRYNCASYSTEM_H_ */
