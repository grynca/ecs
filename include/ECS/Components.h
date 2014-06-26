#ifndef GRYNCACOMPONENTS_H
#define GRYNCACOMPONENTS_H

#include "TypeIdMask.h"
#include "CommonDefs.h"

namespace Grynca {

    template<typename ...Comps>
    class Components {
    public:
        static ComponentsMaskBits getComponentsMask() {
            static TypeIdMask<MAX_COMPONENTS, Comps...> m;
            return m.bits;
        }
    };

    template <typename ...Comps>
    class DynamicComponents {
    public:
        static ComponentsMaskBits getStaticComponentsMask() {
            return ComponentsMaskBits();
        }
        static ComponentsMaskBits getDynamicComponentsMask() {
            return Components<Comps...>::getComponentsMask();
        }
    };

    template <typename ...Comps>
    class StaticComponents {
    public:
        static ComponentsMaskBits getStaticComponentsMask() {
            return Components<Comps...>::getComponentsMask();
        }
        static ComponentsMaskBits getDynamicComponentsMask() {
            return ComponentsMaskBits();
        }
    };

    template <typename ...ComponentPacks> class ComponentMasksCombinator;

    template <>
    class ComponentMasksCombinator<> {
    public:
        static ComponentsMaskBits getStaticComponentsMask() {
            return ComponentsMaskBits();
        }
        static ComponentsMaskBits getDynamicComponentsMask() {
            return ComponentsMaskBits();
        }
    };

    template <typename FirstComponentPack, typename ... Rest>
    class ComponentMasksCombinator<FirstComponentPack, Rest...> : public ComponentMasksCombinator<Rest...> {
    public:
        using Parent = ComponentMasksCombinator<Rest...>;

        static ComponentsMaskBits getStaticComponentsMask() {
            return FirstComponentPack::getStaticComponentsMask() | Parent::getStaticComponentsMask();
        }
        static ComponentsMaskBits getDynamicComponentsMask() {
            return FirstComponentPack::getDynamicComponentsMask() | Parent::getDynamicComponentsMask();
        }
    };
}

#endif // GRYNCACOMPONENTS_H
