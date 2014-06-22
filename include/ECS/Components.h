#ifndef GRYNCACOMPONENTS_H
#define GRYNCACOMPONENTS_H

#include "TypeIdMask.h"
#include "CommonDefs.h"
#include "InternalComponents.h"

namespace Grynca {

    template <typename ... Comps>
    class StaticComponents {
    public:
        static ComponentsMaskBits getStaticComponentsMask() {
            static TypeIdMask<MAX_COMPONENTS, Comps..., EntityTypeHeaderComponent> m;
            return m.bits;
        }
    };

    template <typename ... Comps>
    class DynamicComponents {
    public:
        static ComponentsMaskBits getDynamicComponentsMask() {
            static TypeIdMask<MAX_COMPONENTS, Comps..., EntityHeaderComponent> m;
            return m.bits;
        }
    };
}

#endif // GRYNCACOMPONENTS_H
