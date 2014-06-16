/*
 * EntityType.h
 *
 *  Created on: 8. 1. 2014
 *      Author: lutza
 */

#ifndef ENTITIES_REGISTER_H
#define ENTITIES_REGISTER_H

#include <vector>
#include <cassert>
#include <stdint.h>
#include <string>
#include <typeinfo>
#include "CommonDefs.h"
#include "ComponentsRegister.h"
#include "Offsets.h"

namespace Grynca {

    struct EntityTypeInfo {
        EntityTypeInfo() : type_id(-1), entity_size(0) {}

        bool isRegistered()const { return type_id!=-1; }

        int type_id;
        std::string name;
        size_t entity_size;
        ComponentsMaskBits components_mask; // mask of components that entity consists of - not changeable
        std::vector<int> component_offsets;  // for not contained components it is -1
        DestroyFunc destroy_func;
    };

    class EntitiesRegister {
    public:
        static const EntityTypeInfo& getEntityTypeInfo(unsigned int type_id) {
            assert(type_id < register_().size());
            return register_()[type_id];
        }

        static size_t registeredTypesCount() { return register_().size(); }

        static void registerEntityType(const unsigned int entity_type_id,
                                       const std::string& name,
                                       const size_t entity_size,
                                       const ComponentsMaskBits& components_mask,
                                       const std::vector<int>& component_offsets,
                                       DestroyFunc destroy_func)
        {
            if (entity_type_id >= register_().size()) {
                register_().resize(entity_type_id+1);
            }
            assert(!register_()[entity_type_id].isRegistered() && "Entity type with this type-id is already registered.");
#ifndef	NDEBUG
            std::cout << "[EntitiesRegister] Registering entity " << name << " to id " << entity_type_id << std::endl;
#endif
            register_()[entity_type_id].type_id = entity_type_id;
            register_()[entity_type_id].name = name;
            register_()[entity_type_id].entity_size = entity_size;
            register_()[entity_type_id].components_mask = components_mask;
            register_()[entity_type_id].component_offsets = component_offsets;
            register_()[entity_type_id].destroy_func = destroy_func;

//            for (unsigned int i=0; i<register_()[entity_type_id].component_offsets.size(); i++ ) {
//                 std::cout << "offset " <<i<<" "<< register_()[entity_type_id].component_offsets[i] << std::endl;
//            }
        }

    protected:
        // Static register for entity types
        static std::vector<EntityTypeInfo>& register_() {
            static std::vector<EntityTypeInfo> reg;
            return reg;
        }
    };

    template <typename Derived>
    static const EntityTypeInfo& registerEntityType() {
        EntitiesRegister::registerEntityType(Derived::typeId,
                                             typeid(Derived).name(),
                                             sizeof(Derived),
                                             Derived::getComponentsMask().bits,
                                             Grynca::Offsets::getOffsets<typename Derived::ComponentsTuple>(MAX_COMPONENTS),
                                             Derived::destroy);
        return EntitiesRegister::getEntityTypeInfo(Derived::typeId);
    }

    template <typename Derived>
    class EntityType {
    public:

        // default destroy function, calls Derived class destructor,
        //  can be shadowed in derived class
        static void destroy(void* data) {
            ((Derived*)data)->~Derived();
        }

        static const Grynca::EntityTypeInfo& getTypeInfoStatic()
        {
            return EntitiesRegister::getEntityTypeInfo(Derived::typeId);
        }
    };
}

#define REGISTER_ENTITY_TYPE(EType) static class EType##_Registrator_{ \
    public: \
    EType##_Registrator_() { \
        Grynca::registerEntityType<EType>(); \
    } \
} EType##registrator_;


#endif /* ENTITIES_REGISTER_H */
