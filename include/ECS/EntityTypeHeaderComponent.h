#ifndef GRYNCAENTITYTYPEHEADERCOMPONENT_H
#define GRYNCAENTITYTYPEHEADERCOMPONENT_H

#include "InternalComponents.h"
#include "Component.h"

namespace Grynca {

    // static internal component of every entity
    class EntityTypeHeaderComponent : public Component<EntityTypeHeaderComponent>
    {
    public:
        static const unsigned int typeId = InternalComponentIds::EntityTypeHeaderComponentId;

        template <typename SysType>
        void setRelevantSystem(bool set) { relevant_systems[SysType::typeId] = set; }


        // systems allowed to update this entity type (must be compatible with its components)
        // initially all systems are non-relevant and relevantness must be set in staticInit()
        SystemsMaskBits relevant_systems;
    };
    REGISTER_COMPONENT_TYPE(EntityTypeHeaderComponent);
}

#endif // GRYNCAENTITYTYPEHEADERCOMPONENT_H
