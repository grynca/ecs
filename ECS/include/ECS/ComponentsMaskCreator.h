/*
 * ComponentsMaskCreator.h
 *
 *  Created on: 21. 12. 2013
 *      Author: lutze
 */

#ifndef COMPONENTSMASKCREATOR_H_
#define COMPONENTSMASKCREATOR_H_

#include "CommonDefs.h"

namespace Grynca
{
	// creates components mask from template args
	template <typename... Comps> class ComponentsMaskCreator;

	// specialization for recursion bottom
	template<>
	class ComponentsMaskCreator<>
	{
	public:
		ComponentsMask mask;
	};

	// general specialization
	template <typename FirstC, typename... Rest>
	class ComponentsMaskCreator<FirstC, Rest...> : public ComponentsMaskCreator<Rest...>
	{
		typedef ComponentsMaskCreator<Rest...> Parent;
	public:
		ComponentsMaskCreator()
		{
			assert(FirstC::familyId() != -1
				   && "Component must be registered.");
			Parent::mask[FirstC::familyId()] = 1;
		}
	};
}




#endif /* COMPONENTSMASKCREATOR_H_ */
