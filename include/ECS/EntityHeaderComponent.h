#ifndef ENTITYHEADERCOMPONENT_H
#define ENTITYHEADERCOMPONENT_H

#include "ComponentsRegister.h"
#include "Component.h"

namespace Grynca {

    class EntityHeaderComponent : public Component<EntityHeaderComponent>
    {
    public:
        static const unsigned int typeId = 0;

        uint32_t guid;
        unsigned int pool_chunk;
        unsigned int chunk_position;
    };

    REGISTER_COMPONENT_TYPE(EntityHeaderComponent);
}

#endif // ENTITYHEADERCOMPONENT_H
