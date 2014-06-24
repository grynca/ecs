#ifndef GRYNCAINTERNALCOMPONENTS_H
#define GRYNCAINTERNALCOMPONENTS_H

namespace Grynca {

    enum ECSInternalComponentIds {
        EntityTypeHeaderComponentId = 0,
        EntityHeaderComponentId = 1,
    // ====================================
        ECSInternalComponentIdsEnd
    };

}

#include "EntityHeaderComponent.h"
#include "EntityTypeHeaderComponent.h"

#endif // GRYNCAINTERNALCOMPONENTS_H
