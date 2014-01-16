/*
 * main.cpp
 *
 *  Created on: 22. 12. 2013
 *      Author: lutze
 */

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <math.h>
#include "ECS.h"


class PositionComponent : public Grynca::Component<PositionComponent>
{
public:
	PositionComponent(float px, float py, float r) : x(px), y(py), rot(r) {}
	~PositionComponent()
	{
		//std::cout << "Destroying position component." << std::endl;
	}

	float x, y;
	float rot;
};

class VelocityComponent : public Grynca::Component<VelocityComponent>
{
public:
	VelocityComponent(float lv_x, float lv_y, float av) : lv_x(lv_x), lv_y(lv_y), av(av) {}

	float lv_x, lv_y;
	float av;
};

class MovementSystem : public Grynca::System<MovementSystem, PositionComponent, VelocityComponent>
{
public:
	void updateImpl(float dt, Grynca::UpdateIterator& it)
	{
		while (it.next())
		{
			Grynca::Entity& e = it.getEntity();
			PositionComponent& pos = it.get<PositionComponent>();
			VelocityComponent& vel = it.get<VelocityComponent>();

			pos.x += vel.lv_x * dt;
			pos.y += vel.lv_y *dt;
			pos.rot += vel.av *dt;

			//std::cout << "[MovementSystem] Updating position for entity: " << e.guid() << std::endl;
			//std::cout << "		p=[" << pos.x << ", " << pos.y << "], rot=" << pos.rot << std::endl;
		}
	}
};


class TestEntity : public Grynca::EntityType<TestEntity>
{
public:
	static Grynca::Entity* create(Grynca::ECSManager& manager,
								  float x_pos, float y_pos, float rot,
								  float x_vel, float y_vel, float ang_vel)
	{
		Grynca::Entity* newent = manager.entities().addEntity(entityTypeId());
		// add initial components
		PositionComponent* pos = manager.entities().getComponent<PositionComponent>(newent);
		VelocityComponent* vel = manager.entities().getComponent<VelocityComponent>(newent);
		*pos = {x_pos, y_pos, rot};
		*vel = {x_vel, y_vel, ang_vel};

		return newent;
	}

	static const Grynca::ComponentsMask& componentsMask()
	{
		static Grynca::ComponentsMask mask = Grynca::ComponentsMaskCreator<PositionComponent, VelocityComponent>().mask;
		return mask;
	}

	static const Grynca::SystemsMask& initialUpdateMask()
	{
		static Grynca::SystemsMask mask = Grynca::SystemsMaskCreator<MovementSystem>().mask;
		return mask;
	}
};



class MyEntityManager : public Grynca::ECSManager
{
public:

	MyEntityManager()
	{
		// register components
		componentsRegister().registerComponent<PositionComponent>();
		componentsRegister().registerComponent<VelocityComponent>();

		// register systems ( must be after components! )s
		systems().registerSystem(new MovementSystem());

		// register entities
		entities().registerEntityType<TestEntity>(1024);

	}
};

int main(int argc, char* argv[])
{
	MyEntityManager em;

	for (unsigned int i=0; i<1000000; i++)
	{
		float rand_rot = (float)(rand()%360)/180*3.14;
		Grynca::Entity* new_entity = TestEntity::create(em, i, i*10, rand_rot,
								  	  	  	  	        i, (float)i/2, 0.1f*i);
		//std::cout << "Added entity: " << new_entity->guid() << std::endl;
	}

	float run_time = 0.0f;
	clock_t last_clock = clock();
	while(run_time < 10.0f)
	{
		// wait at least for one millisecond elapsed
		float dt;
		clock_t now;
		do
		{
			now = clock();
			dt = (float)(now-last_clock)/CLOCKS_PER_SEC;
		}
		while (dt < 0.001f);
		last_clock = now;
		std::cout << "dt= " << dt << std::endl;
		// update all systems, in order as they were registered
		em.updateAll(dt);
		run_time += dt;
	}

	std::cout << "Ended" << std::endl;
	return EXIT_SUCCESS;
}


