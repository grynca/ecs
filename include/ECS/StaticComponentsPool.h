#ifndef GRYNCASTATICCOMPONENTSPOOL_H
#define GRYNCASTATICCOMPONENTSPOOL_H

#include <vector>
#include <cassert>

namespace Grynca {

    // fw
    class EntityTypeInfo;

    class StaticComponentsPool {
    public:
        StaticComponentsPool(EntityTypeInfo& eti);

        void* get(unsigned int component_type_id);

        template<typename CompType>
        CompType* get();

        bool contains(unsigned int component_type_id);
    private:
        // array for static components
        std::vector<uint8_t> data_;
        // where in array does each component start,
        //      -1 if component is not contained
        std::vector<int> component_starts_;
    };
}

#include "EntitiesRegister.h"
#include "ComponentsRegister.h"

namespace Grynca {

    inline StaticComponentsPool::StaticComponentsPool(EntityTypeInfo& eti) {
        // gather static components info
        int static_comps_size = 0;
        for (unsigned int i=0; i<eti.staticComponentsMask().size(); ++i) {
            if (eti.staticComponentsMask()[i]) {
                component_starts_.push_back(static_comps_size);
                static_comps_size += (int)ComponentsRegister::get(i).component_size;
            }
            else
                // component not present in entity
                component_starts_.push_back(-1);
        }
        data_.resize(static_comps_size);
    }

    inline void* StaticComponentsPool::get(unsigned int component_type_id) {
        assert(component_type_id<MAX_COMPONENTS);
        assert(contains(component_type_id)
               && "This entity type does not contain required static component.");
        return &(data_[component_starts_[component_type_id]]);
    }

    template<typename CompType>
    inline CompType* StaticComponentsPool::get() {
        return (CompType*)get(CompType::typeId);
    }

    inline bool StaticComponentsPool::contains(unsigned int component_type_id) {
        return component_starts_[component_type_id]>=0;
    }
}
#endif // GRYNCASTATICCOMPONENTSPOOL_H
