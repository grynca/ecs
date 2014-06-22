#ifndef GRYNCATYPEIDMASK_H
#define GRYNCATYPEIDMASK_H

#include "CommonDefs.h"
#include "ComponentsRegister.h"

namespace Grynca {
    template <unsigned int bits_count, typename... Types> class TypeIdMask;
    // specialization for recursion bottom
    template<unsigned int bits_count>
    class TypeIdMask<bits_count>
    {
    public:
        std::bitset<bits_count> bits;
    };
    // general specialization
    template <unsigned int bits_count, typename FirstC, typename... Rest>
    class TypeIdMask<bits_count, FirstC, Rest...> : public TypeIdMask<bits_count, Rest...>
    {
        typedef TypeIdMask<bits_count, Rest...> Parent;
    public:
        TypeIdMask()
        {
            Parent::bits.set(FirstC::typeId);
        }
    };
}

#endif // GRYNCATYPEIDMASK_H
