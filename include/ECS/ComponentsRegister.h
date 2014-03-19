/*
 * ComponentsRegister.h
 *
 *  Created on: 7. 1. 2014
 *      Author: lutza
 */

#ifndef COMPONENTSREGISTER_H_
#define COMPONENTSREGISTER_H_

#include <cstddef>
#include <vector>
#include <cassert>
#include "EcsConfig.h"

namespace Grynca
{
	class ComponentsRegister
	{
		typedef void (*DestructorFunc)(uint8_t*);
	public:
		template <typename Comp>
		void registerComponent();

		unsigned int componentSize(unsigned int component_family_id);

		DestructorFunc componentDestructor(unsigned int component_family_id);

		unsigned int registeredComponentsCount();
	private:
		// internal struct
		struct ComponentInfo
		{
			unsigned int component_size;
			DestructorFunc destructor;
		};
		std::vector<ComponentInfo> _comps_info;
	};
}

template <typename Comp>
inline void Grynca::ComponentsRegister::registerComponent()
{
	assert(Comp::familyId() == -1
			&& "Component is already registered.");
	assert(_comps_info.size() < MAX_COMPONENTS
			&& "You have too many components, change MAX_COMPONENTS if necessary.");

	Comp::_familyId() = _comps_info.size();
	_comps_info.push_back( {sizeof(Comp), Comp::_destructor} );
}


inline unsigned int Grynca::ComponentsRegister::componentSize(unsigned int component_family_id)
{
	assert(component_family_id < _comps_info.size());
	return _comps_info[component_family_id].component_size;
}

inline Grynca::ComponentsRegister::DestructorFunc Grynca::ComponentsRegister::componentDestructor(unsigned int component_family_id)
{
	assert(component_family_id < _comps_info.size());
	return _comps_info[component_family_id].destructor;
}

inline unsigned int Grynca::ComponentsRegister::registeredComponentsCount()
{
	return _comps_info.size();
}



#endif /* COMPONENTSREGISTER_H_ */
