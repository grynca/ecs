/*
 * ComponentsRegister.h
 *
 *  Created on: 7. 1. 2014
 *      Author: lutza
 */

#ifndef COMPONENTSREGISTER_H
#define COMPONENTSREGISTER_H

#include <vector>
#include <cassert>
#include <stdint.h>
#include <string>
#include <typeinfo>
#include "CommonDefs.h"

namespace Grynca {

    class ComponentTypeInfo {
    public:
        ComponentTypeInfo() : type_id(-1), component_size(0) {}

        bool isRegistered()const { return type_id!=-1; }

        int type_id;
        std::string name;
        size_t component_size;
    };

    class ComponentsRegister {
    public:
        static const ComponentTypeInfo& getComponentTypeInfo(unsigned int type_id) {
            assert(type_id < register_().size());
            return register_()[type_id];
        }
        static size_t registeredComponentTypesCount() { return register_().size(); }

        static void registerComponentType(const unsigned int component_type_id,
                                          const std::string& name,
                                          const size_t component_size)
        {
            if (component_type_id >= register_().size()) {
                register_().resize(component_type_id+1);
            }
            assert(!register_()[component_type_id].isRegistered() && "Component type with this type-id is already registered.");
#ifndef NDEBUG
            std::cout << "[ComponentsRegister] Registering component name " << name << " to id " << component_type_id << std::endl;
#endif
            register_()[component_type_id].type_id = component_type_id;
            register_()[component_type_id].name = name;
            register_()[component_type_id].component_size = component_size;
        }

    protected:
        // Static register for component types
        static std::vector<ComponentTypeInfo>& register_() {
            static std::vector<ComponentTypeInfo> reg;
            return reg;
        }
    };

    template <typename Derived>
    static const ComponentTypeInfo& registerComponentType() {
        ComponentsRegister::registerComponentType(Derived::componentTypeId,
                                                  typeid(Derived).name(),
                                                  sizeof(Derived));
        return ComponentsRegister::getComponentTypeInfo(Derived::componentTypeId);
    }
}

// must access typeInfo (static member in templated class) or compiler will optimize it out
#define REGISTER_COMPONENT_TYPE(CType) static class CType##_Registrator_{ \
    public: \
    CType##_Registrator_() { \
        Grynca::registerComponentType<CType>(); \
    } \
} CType##registrator_;


#endif /* COMPONENTSREGISTER_H */
