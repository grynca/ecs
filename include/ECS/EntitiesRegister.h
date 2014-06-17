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

    class EntityTypeInfo {
    public:
        EntityTypeInfo() : type_id_(-1), entity_size_(0), destroyFunc(NULL) {}

        bool isRegistered()const { return type_id_!=-1; }

        int getTypeId() { return type_id_; }
        const std::string& getName() { return name_; }
        size_t getEntitySize() { return entity_size_; }
        const ComponentsMaskBits& getComponentsMask() { return components_mask_; }
        const std::vector<int>& getComponentOffsets() { return component_offsets_; }
        std::vector<bool>& relevantSystems() { return relevant_systems_; }
        void setRelevantSystem(unsigned int system_type_id) { setRelevantSystem_(system_type_id, true); }
        void unsetRelevantSystem(unsigned int system_type_id) { setRelevantSystem_(system_type_id, false); }
        template <typename SystemType>
        void setRelevantSystem() { setRelevantSystem_(SystemType::systemTypeId, true); }
        template <typename SystemType>
        void unsetRelevantSystem() { unsetRelevantSystem_(SystemType::systemTypeId, false); }


        DestroyFunc destroyFunc;
    private:
        friend class EntitiesRegister;
        void set_(int type_id, const std::string&name, size_t entity_size,
                 const ComponentsMaskBits& components_mask, std::vector<int>& component_offsets,
                 DestroyFunc destroy_func)
        {
            destroyFunc = destroy_func;
            type_id_ = type_id;
            name_ = name;
            entity_size_ = entity_size;
            components_mask_ = components_mask;
            component_offsets_ = component_offsets;
        }

        void setRelevantSystem_(unsigned int system_type_id, bool set) {
            if (system_type_id >= relevant_systems_.size())
                relevant_systems_.resize(system_type_id+1, false);
            relevant_systems_[system_type_id] = set;
        }

        int type_id_;
        std::string name_;
        size_t entity_size_;
        ComponentsMaskBits components_mask_; // mask of components that entity consists of - not changeable
        std::vector<int> component_offsets_;  // for not contained components it is -1
        std::vector<bool> relevant_systems_;
    };

    class EntitiesRegister {
    public:
        static EntityTypeInfo& getEntityTypeInfo(unsigned int type_id) {
            assert(type_id < register_().size());
            return register_()[type_id];
        }

        static size_t registeredTypesCount() { return register_().size(); }

        template <typename Derived>
        static void registerEntityType()
        {
            if (Derived::entityTypeId >= register_().size()) {
                register_().resize(Derived::entityTypeId+1);
            }
            assert(!register_()[Derived::entityTypeId].isRegistered() && "Entity type with this type-id is already registered.");
#ifndef	NDEBUG
            std::cout << "[EntitiesRegister] Registering entity " << typeid(Derived).name() << " to id " << Derived::entityTypeId << std::endl;
#endif
            register_()[Derived::entityTypeId].set_(Derived::entityTypeId,
                                            typeid(Derived).name(),
                                            sizeof(Derived),
                                            Derived::getComponentsMask().bits,
                                            Grynca::Offsets::getOffsets<typename Derived::ComponentsTuple>(MAX_COMPONENTS),
                                            Derived::destroy);
//            for (unsigned int i=0; i<register_()[entity_type_id].component_offsets.size(); i++ ) {
//                 std::cout << "offset " <<i<<" "<< register_()[entity_type_id].component_offsets[i] << std::endl;
//            }
            // call entityType's staticSetup
            Derived::staticSetup();
        }

    protected:
        // Static register for entity types
        static std::vector<EntityTypeInfo>& register_() {
            static std::vector<EntityTypeInfo> reg;
            return reg;
        }
    };

    template <typename Derived>
    static EntityTypeInfo& registerEntityType() {
        EntitiesRegister::registerEntityType<Derived>();
        // call entityType's staticSetup
        Derived::staticSetup();
        // return entityTypeInfo
        return EntitiesRegister::getEntityTypeInfo(Derived::entityTypeId);
    }

    template <typename Derived>
    class EntityType {
    public:
        // default staticSetup, called after entityType registration
        //  can be shadowed in derived class
        static void staticSetup() {}

        // default destroy function, calls Derived class destructor,
        //  can be shadowed in derived class
        static void destroy(void* data) {
            ((Derived*)data)->~Derived();
        }

        static Grynca::EntityTypeInfo& getTypeInfoStatic()
        {
            return EntitiesRegister::getEntityTypeInfo(Derived::entityTypeId);
        }

    private:
    };
}

#define REGISTER_ENTITY_TYPE(EType) static class EType##_Registrator_{ \
    public: \
    EType##_Registrator_() { \
        Grynca::registerEntityType<EType>(); \
    } \
} EType##registrator_;


#endif /* ENTITIES_REGISTER_H */
