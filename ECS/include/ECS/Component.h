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
		friend class ComponentsRegister;
	public:
		static int familyId();
	private:
		static int& _familyId();
		// explicitly calls components destructor ( component is created with placement new )
		// called by Components
		static void _destructor(uint8_t* comp);
	};
}

template<typename Derived>
inline int Grynca::Component<Derived>::familyId()
// static
{
	return _familyId();
}

template<typename Derived>
inline int& Grynca::Component<Derived>::_familyId()
// static
{
	static int _family_id = -1;		// -1 means not registered
	return _family_id;
}

template<typename Derived>
inline void Grynca::Component<Derived>::_destructor(uint8_t* comp)
// static
{
	assert(comp);
	// call destructor explicitly
	((Derived*)comp)->~Derived();
}

#endif /* COMPONENT_H_ */
