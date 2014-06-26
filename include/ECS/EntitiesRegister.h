/*
 * EntityType.h
 *
 *  Created on: 8. 1. 2014
 *      Author: lutza
 */

#ifndef GRYNCAENTITIES_REGISTER_H
#define GRYNCAENTITIES_REGISTER_H

#include <vector>
#include <cassert>
#include <stdint.h>
#include <string>
#include <typeinfo>
#include "CommonDefs.h"
#include "ComponentsRegister.h"

namespace Grynca {
    //fw
    class StaticComponentsPool;
    class ECSManager;

    typedef void (*StaticInitFunc)(ECSManager&, StaticComponentsPool&);

    class EntityTypeInfo {
    public:
        EntityTypeInfo() : type_id_(-1) {}

        bool isRegistered()const { return type_id_!=-1; }
        int typeId()const { return type_id_; }
        const std::string& name() { return name_;}
        const ComponentsMaskBits& staticComponentsMask()const { return static_components_mask_; }
        const ComponentsMaskBits& dynamicComponentsMask()const { return dynamic_components_mask_; }
        const ComponentsMaskBits& allComponentsMask()const { return all_components_mask_; }
        DestroyFunc destroyFunc() { return destroy_func_; }
        StaticInitFunc staticInitFunc() { return static_init_func_; }
    private:
        friend class EntitiesRegister;
        int type_id_;
        std::string name_;
        ComponentsMaskBits static_components_mask_;
        ComponentsMaskBits dynamic_components_mask_;
        ComponentsMaskBits all_components_mask_;
        DestroyFunc destroy_func_;
        StaticInitFunc static_init_func_;
    };

    class EntitiesRegister {
    public:
        static EntityTypeInfo& get(unsigned int type_id) {
            assert(type_id < register_().size());
            return register_()[type_id];
        }

        static size_t registeredTypesCount() { return register_().size(); }

        template <typename Derived>
        static void registerEntityType(const std::string& name)
        {
            if (isRegistered(name))
                // when including .h with REGISTER macro in multiple translation units
                return;
            if (Derived::typeId >= register_().size()) {
                register_().resize(Derived::typeId+1);
            }
            assert(!register_()[Derived::typeId].isRegistered()
                   && "Entity type with this type-id is already registered.");
#ifndef	NDEBUG
            std::cout << "[EntitiesRegister] Registering entity " << name << " to id " << Derived::typeId << std::endl;
#endif
            register_()[Derived::typeId].type_id_ = Derived::typeId;
            register_()[Derived::typeId].name_ = name;
            register_()[Derived::typeId].static_components_mask_ = Derived::getStaticComponentsMask();
            register_()[Derived::typeId].dynamic_components_mask_ = Derived::getDynamicComponentsMask();
            register_()[Derived::typeId].all_components_mask_ = Derived::getStaticComponentsMask() | Derived::getDynamicComponentsMask();
            register_()[Derived::typeId].destroy_func_ = Derived::destroy;
            register_()[Derived::typeId].static_init_func_ = Derived::staticInit;

#ifndef	NDEBUG
            std::cout << " Static Components: " <<std::endl
                      << " " << Derived::getStaticComponentsMask() << std::endl;
            std::cout << " Dynamic Components: " <<std::endl
                      << " " << Derived::getDynamicComponentsMask() << std::endl;
#endif
            assert((Derived::getStaticComponentsMask()&Derived::getDynamicComponentsMask())==0
                   && "Entity cant contain bots static and dynamic component of same type.");
        }

        static bool isRegistered(const std::string& type_name) {
            for(unsigned int i=0; i<registeredTypesCount(); i++) {
                if (get(i).name_ == type_name)
                    return true;
            }
            return false;
        }

    protected:
        // Static register for entity types
        static std::vector<EntityTypeInfo>& register_() {
            static std::vector<EntityTypeInfo> reg;
            return reg;
        }
    };
}

#define REGISTER_ENTITY_TYPE(EType) static class EType##_Registrator_{ \
    public: \
    EType##_Registrator_() { \
        Grynca::EntitiesRegister::registerEntityType<EType>(#EType); \
    } \
} EType##registrator_


#endif /* GRYNCAENTITIES_REGISTER_H */
