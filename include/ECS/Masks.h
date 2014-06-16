#ifndef MASKS_H
#define MASKS_H

#include "CommonDefs.h"
#include "ComponentsRegister.h"

namespace Grynca {
    template <typename... Comps> class ComponentsMask;
    // specialization for recursion bottom
    template<>
    class ComponentsMask<>
    {
    public:
        ComponentsMaskBits bits;
    };
    // general specialization
    template <typename FirstC, typename... Rest>
    class ComponentsMask<FirstC, Rest...> : public ComponentsMask<Rest...>
    {
        typedef ComponentsMask<Rest...> Parent;
    public:
        ComponentsMask()
        {
            assert(ComponentsRegister::getComponentTypeInfo(FirstC::typeId).isRegistered()
                    && "Component must be registered.");
            Parent::bits.set(FirstC::getTypeInfoStatic().type_id);
        }
    };
}

#endif // MASKS_H
