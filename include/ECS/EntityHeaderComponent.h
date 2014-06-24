#ifndef GRYNCAENTITYHEADERCOMPONENT_H
#define GRYNCAENTITYHEADERCOMPONENT_H

#include "InternalComponents.h"
#include "Component.h"

namespace Grynca {

    class EntityHeaderComponent : public Component<EntityHeaderComponent>
    {
    public:
        static const unsigned int typeId = ECSInternalComponentIds::EntityHeaderComponentId;

        uint32_t guid;
        // systems currently updating this entity
        SystemsMaskBits update_mask;
    };
    REGISTER_COMPONENT_TYPE(EntityHeaderComponent);
}

#endif // GRYNCAENTITYHEADERCOMPONENT_H
