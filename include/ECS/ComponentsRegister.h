/*
 * ComponentsRegister.h
 *
 *  Created on: 7. 1. 2014
 *      Author: lutza
 */

#ifndef GRYNCACOMPONENTSREGISTER_H
#define GRYNCACOMPONENTSREGISTER_H

#include <vector>
#include <cassert>
#include <stdint.h>
#include <string>
#include <typeinfo>
#include <iostream>
#include "CommonDefs.h"

namespace Grynca {

    class ComponentTypeInfo {
    public:
        ComponentTypeInfo() : type_id(-1) {}

        bool isRegistered()const { return type_id!=-1; }

        int type_id;
        std::string name;
        size_t component_size;
        DestroyFunc destroy_func;
    };

    class ComponentsRegister {
    public:
        static const ComponentTypeInfo& get(unsigned int type_id) {
            assert(type_id < register_().size());
            return register_()[type_id];
        }
        static size_t registeredTypesCount() { return register_().size(); }

        template <typename Derived>
        static void registerComponentType(const std::string& name)
        {
            if (isRegistered(name))
                // when including .h with REGISTER macro in multiple translation units
                return;
            if (Derived::typeId >= register_().size()) {
                register_().resize(Derived::typeId+1);
            }
            assert(!register_()[Derived::typeId].isRegistered()
                    && "Component type with this type-id is already registered.");
#ifndef NDEBUG
            std::cout << "[ComponentsRegister] Registering component " << name << " to id " << Derived::typeId << std::endl;
#endif
            register_()[Derived::typeId].type_id = Derived::typeId;
            register_()[Derived::typeId].name = name;
            register_()[Derived::typeId].component_size = sizeof(Derived);
            register_()[Derived::typeId].destroy_func = Derived::destroy;
        }

        static bool isRegistered(const std::string& type_name) {
            for(unsigned int i=0; i<registeredTypesCount(); i++) {
                if (get(i).name == type_name)
                    return true;
            }
            return false;
        }

    protected:
        // Static register for component types
        static std::vector<ComponentTypeInfo>& register_() {
            static std::vector<ComponentTypeInfo> reg;
            return reg;
        }
    };
}

#define REGISTER_COMPONENT_TYPE(CType) static class CType##_Registrator_{ \
    public: \
    CType##_Registrator_() { \
        Grynca::ComponentsRegister::registerComponentType<CType>(#CType); \
    } \
} CType##registrator_


#endif /* GRYNCACOMPONENTSREGISTER_H */
