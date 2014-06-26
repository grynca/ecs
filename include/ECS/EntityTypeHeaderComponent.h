#ifndef GRYNCAENTITYTYPEHEADERCOMPONENT_H
#define GRYNCAENTITYTYPEHEADERCOMPONENT_H

#include "InternalComponents.h"
#include "Component.h"

namespace Grynca {

    // static internal component of every entity
    class EntityTypeHeaderComponent : public Component<EntityTypeHeaderComponent>
    {
    public:
        static const unsigned int typeId = ECSInternalComponentIds::EntityTypeHeaderComponentId;


        template <typename System> void setRelevantSystems(bool set=true)
        {
            relevant_systems[System::typeId] = set;
        }

        template <typename FirstSystem, typename SecondSystem, typename ...Rest>
        void setRelevantSystems(bool set=true)
        {
            setRelevantSystems<FirstSystem>(set);
            setRelevantSystems<SecondSystem, Rest...>(set);
        }


        // systems allowed to update this entity type (must be compatible with its components)
        // initially all systems are non-relevant and relevantness must be set in staticInit()
        SystemsMaskBits relevant_systems;
    };
    REGISTER_COMPONENT_TYPE(EntityTypeHeaderComponent);
}

#endif // GRYNCAENTITYTYPEHEADERCOMPONENT_H
