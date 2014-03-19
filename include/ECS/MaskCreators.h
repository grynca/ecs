/*
 * MaskCreators.h
 *
 *  Created on: 21. 12. 2013
 *      Author: lutze
 */

#ifndef MASKCREATORS_H_
#define MASKCREATORS_H_

#include "CommonDefs.h"

namespace Grynca
{
/// creates components mask from template args
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

/// creates systems mask from template args
	template <typename... Syss> class SystemsMaskCreator;

	// specialization for recursion bottom
	template<>
	class SystemsMaskCreator<>
	{
	public:
		SystemsMask mask;
	};

	// general specialization
	template <typename FirstS, typename... Rest>
	class SystemsMaskCreator<FirstS, Rest...> : public SystemsMaskCreator<Rest...>
	{
		typedef SystemsMaskCreator<Rest...> Parent;
	public:
		SystemsMaskCreator()
		{
			assert(FirstS::systemTypeId() != -1
				   && "System must be registered.");
			Parent::mask[FirstS::systemTypeId()] = 1;
		}
	};
}




#endif /* MASKCREATORS_H_ */
