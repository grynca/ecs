/*
 * Component.h
 *
 *  Created on: 21. 12. 2013
 *      Author: lutze
 */

#ifndef COMPONENT_H_
#define COMPONENT_H_

#include <cassert>
#include "EcsConfig.h"

namespace Grynca
{
	// TODO: some way to execute code on destruction of components
	//		(they are in managed memory and destructors are not called)
	template<typename Derived>
	class Component
	{
		friend class Components;
	public:
		static int familyId();
	private:
		static void _setFamilyId(unsigned int f_id);
		static int& _familyId();
	};
}

template<typename Derived>
inline int Grynca::Component<Derived>::familyId()
// static
{
	return _familyId();
}

template<typename Derived>
inline void Grynca::Component<Derived>::_setFamilyId(unsigned int f_id)
// static
{
	assert(f_id < MAX_COMPONENTS
			&& "You have too many components, change MAX_COMPONENTS if necessary.");
	_familyId() = f_id;
}

template<typename Derived>
inline int& Grynca::Component<Derived>::_familyId()
// static
{
	static int _family_id = -1;		// -1 means not registered
	return _family_id;
}



#endif /* COMPONENT_H_ */
