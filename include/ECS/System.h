/*
 * System.h
 *
 *  Created on: 21. 12. 2013
 *      Author: lutze
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <cassert>
#include <functional>
#include "CommonDefs.h"

namespace Grynca {
    // fw
    class EntitiesPool;
    class EntityBase;

    class SystemBase {
    public:
        bool isPoolCompatible(const EntitiesPool& pool);

        // pool must be compatible
        void updatePool(double dt, EntitiesPool& pool);

        unsigned int systemId() { return system_id_; }
        const ComponentsMaskBits& neededComponentsMask() { return needed_components_; }
    protected:
        SystemBase() {}

        virtual ~SystemBase() {}

        std::function<void(double, EntityBase*)> update_func_;
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

//        // dummy create, can be shadowed with derived class
//        void create() {}

        virtual ~System() {}
    };

}

#include "EntitiesPool.h"
#include "Entity.h"
#include "Masks.h"

namespace Grynca {
    inline bool SystemBase::isPoolCompatible(const EntitiesPool &pool) {
        return (pool.getEntityTypeInfo().components_mask & needed_components_) == needed_components_;
    }

    // pool must be compatible
    inline void SystemBase::updatePool(double dt, EntitiesPool& pool) {
        pool.loopEntities(dt, update_func_);
    }

    template <typename Derived, typename... Comps>
    inline System<Derived, Comps...>::System()
    {
        update_func_ = std::bind(&Derived::updateEntity, (Derived*)this, std::placeholders::_1, std::placeholders::_2);
        needed_components_ = Grynca::ComponentsMask<Comps...>().bits;   // construct needed components mask
    }

}


#endif /* SYSTEM_H_ */
