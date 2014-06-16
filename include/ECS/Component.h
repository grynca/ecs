/*
 * Component.h
 *
 *  Created on: 21. 12. 2013
 *      Author: lutze
 */

#ifndef COMPONENT_H_
#define COMPONENT_H_

#include "ComponentsRegister.h"

namespace Grynca {

    class ComponentBase
    {

    };

    template <typename Derived>
    class Component : ComponentBase
    {
    public:
        static const ComponentTypeInfo& getTypeInfoStatic() {
            return ComponentsRegister::getComponentTypeInfo(Derived::typeId);
        }

        // default destruction, can be shadowed and changed in derived class
        static void destroy(void* comp){
            ((Derived*)comp)->~Derived();
        }
    };
}

#endif /* COMPONENT_H_ */
