/*
 * CommonDefs.h
 *
 *  Created on: 22. 12. 2013
 *      Author: lutze
 */

#ifndef GRYNCACOMMONDEFS_H_
#define GRYNCACOMMONDEFS_H_

#include <bitset>
#include "EcsConfig.h"

namespace Grynca
{
    typedef std::bitset<MAX_COMPONENTS> ComponentsMaskBits;
    typedef std::bitset<MAX_SYSTEMS> SystemsMaskBits;
    typedef void (*DestroyFunc)(void*);
}



#endif /* GRYNCACOMMONDEFS_H_ */
