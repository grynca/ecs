/*
 * CommonDefs.h
 *
 *  Created on: 22. 12. 2013
 *      Author: lutze
 */

#ifndef COMMONDEFS_H_
#define COMMONDEFS_H_

#include <bitset>
#include "EcsConfig.h"

namespace Grynca
{
    typedef std::bitset<MAX_COMPONENTS> ComponentsMaskBits;
    typedef void (*DestroyFunc)(void*);
}



#endif /* COMMONDEFS_H_ */
