/*
 * EntityType.h
 *
 *  Created on: 8. 1. 2014
 *      Author: lutza
 */

#ifndef ENTITYTYPE_H_
#define ENTITYTYPE_H_

namespace Grynca
{
	template<typename Derived>
	class EntityType
	{
		friend class Entities;
	public:

		// derivate must declare:
		//		static const ComponentsMask& componentsMask()
		//		static const SystemsMask& initialUpdateMask()		// systems that update entity


		static int entityTypeId();
	private:
		static int& _entityTypeId();
	};
}

template<typename Derived>
inline int Grynca::EntityType<Derived>::entityTypeId()
// static
{
	return _entityTypeId();
}

template<typename Derived>
inline int& Grynca::EntityType<Derived>::_entityTypeId()
// static
{
	static int _entity_type_id = -1;		// -1 means not registered
	return _entity_type_id;
}


#endif /* ENTITYTYPE_H_ */
