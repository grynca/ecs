/*
 * Component.h
 *
 *  Created on: 21. 12. 2013
 *      Author: lutze
 */

#ifndef GRYNCACOMPONENT_H_
#define GRYNCACOMPONENT_H_

#include "ComponentsRegister.h"

namespace Grynca {
    template <typename Derived>
    class Component
    {
    public:
        static const ComponentTypeInfo& getTypeInfo() {
            return ComponentsRegister::get(Derived::typeId);
        }

        // calls component's destructor
        static void destroy(void* comp) {
            (void)comp; // so compiler does not complain
            ((Derived*)comp)->~Derived();
        }
    };
}

#endif /* GRYNCACOMPONENT_H_ */
