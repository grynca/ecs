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

namespace Grynca
{
	class ComponentsRegister
	{
		typedef void (*DestructorFunc)(uint8_t*);
	public:


		template <typename Comp>
		void registerComponent()
		{
			assert(Comp::familyId() == -1
					&& "Component is already registered.");
			assert(_comps_info.size() < MAX_COMPONENTS
					&& "You have too many components, change MAX_COMPONENTS if necessary.");

			Comp::_familyId() = _comps_info.size();
			_comps_info.push_back( {sizeof(Comp), Comp::_destructor} );
		}


		unsigned int componentSize(unsigned int component_family_id)
		{
			assert(component_family_id < _comps_info.size());
			return _comps_info[component_family_id].component_size;
		}

		DestructorFunc componentDestructor(unsigned int component_family_id)
		{
			assert(component_family_id < _comps_info.size());
			return _comps_info[component_family_id].destructor;
		}

		unsigned int registeredComponentsCount()
		{
			return _comps_info.size();
		}
	private:

		struct ComponentInfo
		{
			unsigned int component_size;
			DestructorFunc destructor;
		};

		std::vector<ComponentInfo> _comps_info;
	};
}


#endif /* COMPONENTSREGISTER_H_ */
