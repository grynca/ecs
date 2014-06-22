#ifndef GRYNCAENTITYTYPEPOOL_H
#define GRYNCAENTITYTYPEPOOL_H

#include <vector>
#include <functional>
#include <stdint.h>
#include <string.h>
#include "StaticComponentsPool.h"
#include "DynamicComponentsPool.h"

namespace Grynca {

    // fw
    class EntityTypeInfo;
    class ECSManager;

    class EntityTypePool {
    public:
        EntityTypePool(EntityTypeInfo& eti, ECSManager& manager);

        void setEntityMovedCallback(std::function<void(uint32_t, unsigned int)> moved_func );

        StaticComponentsPool& staticComponents() { return static_components_; }
        DynamicComponentsPool& dynamicComponents() { return dynamic_components_; }

        // returns chunk_id, chunk_pos
        std::pair<unsigned int, unsigned int> createNewEntity();

        void destroyEntity(unsigned int chunk_id, unsigned int chunk_pos);

        EntityTypeInfo& getTypeInfo()const { return *eti_; }
        ECSManager& getManager()const { return *manager_; }
    private:
        StaticComponentsPool static_components_;
        DynamicComponentsPool dynamic_components_;

        EntityTypeInfo* eti_;
        ECSManager* manager_;

        // callback when entity pos is changed (on removal of some other entity)
        std::function<void(uint32_t/*guid*/, unsigned int/*new pos*/)> entity_moved_func_;
    };

}

#include "ComponentsRegister.h"
#include "EntitiesRegister.h"
#include "EntityHeaderComponent.h"
#include "ECSManager.h"


namespace Grynca {

    inline EntityTypePool::EntityTypePool(EntityTypeInfo& eti, ECSManager &manager)
     : eti_(&eti),
       static_components_(eti),
       dynamic_components_(eti),
       manager_(&manager)
    {
    }

    inline void EntityTypePool::setEntityMovedCallback(std::function<void(uint32_t, unsigned int)> moved_func ) {
        entity_moved_func_ = moved_func;
    }

    inline std::pair<unsigned int, unsigned int> EntityTypePool::createNewEntity() {
        return dynamic_components_.createNewEntity();
    }

    inline void EntityTypePool::destroyEntity(unsigned int chunk_id, unsigned int chunk_pos) {
        int32_t moved_guid = dynamic_components_.destroyEntity(chunk_id, chunk_pos);
        if (moved_guid >= 0 && entity_moved_func_ != nullptr) {
            // notify someone about entity's moving
            entity_moved_func_(moved_guid, chunk_pos);
        }
    }
}



#endif // GRYNCAENTITYTYPEPOOL_H
