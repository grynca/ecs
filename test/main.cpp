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
#include <limits>
#include <list>
#include "ECS.h"

float randf(float max) {
    return static_cast<float>(rand())/static_cast<float>(RAND_MAX/max);
}

class PositionComponent : public Grynca::Component<PositionComponent>
{
public:
    static const unsigned int componentTypeId = 1;

    PositionComponent() {
        //std::cout << "Constructing position component. " << std::endl;
    }

    void set(double px, double py, double r) {
        x = px;
        y = py;
        rot = r;
    }

    ~PositionComponent()
    {
        //std::cout << "Destroying position component." << std::endl;
    }

    double x, y;
    double rot;
};
REGISTER_COMPONENT_TYPE(PositionComponent);

class VelocityComponent : public Grynca::Component<VelocityComponent>
{
public:
    static const unsigned int componentTypeId = 2;

    void set(double lv_x, double lv_y, double av) {
        this->lv_x = lv_x;
        this->lv_y = lv_y;
        this->av = av;
    }

    ~VelocityComponent()
    {
        ////std::cout << "Destroying velocity component." << std::endl;
    }

    double lv_x, lv_y;
    double av;
};
REGISTER_COMPONENT_TYPE(VelocityComponent);


class MegaComponent : public Grynca::Component<MegaComponent> {
public:
    static const unsigned int componentTypeId = 3;
    int data[300];
};
REGISTER_COMPONENT_TYPE(MegaComponent);

class MovementSystem : public Grynca::System<MovementSystem, PositionComponent, VelocityComponent>
{
public:
    static const unsigned int systemTypeId = 1;

    void updateEntity(double dt, Grynca::EntityBase* entity)
    {
        PositionComponent* pos = entity->get<PositionComponent>();
        VelocityComponent* vel = entity->get<VelocityComponent>();
        pos->x += vel->lv_x * dt;
        pos->y += vel->lv_y *dt;
        pos->rot += vel->av *dt;
        //std::cout << "Movement system updating position of entity " << entity->guid() << std::endl;
    }
};

class TestEntity : public Grynca::Entity<TestEntity,
        // in memory stored reversaly : EntityHeaderComponent, VelocityComponent, PositionComponent ...
                          PositionComponent, VelocityComponent>
{
public:
    static const unsigned int entityTypeId = 1;

    static void staticSetup() {
        getTypeInfoStatic().setRelevantSystem<MovementSystem>();
    }

    TestEntity() {
        //std::cout << "creating test entity" << std::endl;
        get<PositionComponent>()->set(randf(20), randf(20), randf(20));
        get<VelocityComponent>()->set(randf(3), randf(3), randf(3));
    }

    ~TestEntity() {
        //std::cout << "Destroying test entity." << std::endl;
    }
};

REGISTER_ENTITY_TYPE(TestEntity);


int main()
{
    std::cout << "TestEntity size= " << TestEntity::getTypeInfoStatic().getEntitySize() << std::endl;

    unsigned int n_ents = 1000000;
    srand((unsigned int)time(NULL));

    Grynca::ECSManager manager;
    manager.createSystem<MovementSystem>();

    std::list<Grynca::EntityBase*> entities;
    clock_t t = clock();
    for (unsigned int i=0; i<n_ents; i++)
    {
        TestEntity* te = manager.createEntity<TestEntity>();
        entities.push_back(te);
    }

    std::cout << "Creation " << n_ents << " entities in " << (float)(clock()-t)/CLOCKS_PER_SEC << " secs" << std::endl;

    double run_time = 0.0;
    double max_run_time = 10.0;
    unsigned int cycles_count = 0;
    double max_update_time = std::numeric_limits<double>::min();
    double min_update_time = std::numeric_limits<double>::max();

    std::cout << "Updating entities for " << max_run_time << " seconds ..." << std::endl;
    t = clock();

    while(run_time < max_run_time)
    {
        clock_t now = clock();
        double dt = (double)(now-t)/CLOCKS_PER_SEC;
        t = now;

        if (dt > 0)
        {
            if (dt < min_update_time)
                min_update_time = dt;
            if (dt > max_update_time)
                max_update_time = dt;

            manager.updateAll(dt);

            run_time += dt;
            cycles_count++;
        }
    }
    double avg_update_time = run_time/cycles_count;

    std::cout << "Done." << std::endl;
    std::cout << " iteration_time: avg= " << avg_update_time << ", min= "
              << min_update_time << ", max= " << max_update_time << std::endl;


    t = clock();
    for (unsigned int i=0; i<n_ents; i++) {
        manager.destroyEntity(*entities.begin());
        entities.pop_front();;
    }

    std::cout << "Deletion of " << n_ents << " entities in " << (float)(clock()-t)/CLOCKS_PER_SEC << " secs" << std::endl;
    return EXIT_SUCCESS;
}
