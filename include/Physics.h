/**
 * Copyright (c) CTU  - All Rights Reserved
 * Created on: 4/30/20
 *     Author: Vladimir Petrik <vladimir.petrik@cvut.cz>
 *
 *     Wrapper for physics, foundation, cooking, and dispatcher for scenes workload management.
 *     Implements singleton patterns and therefore ensures that physics is created only once.
 */

#ifndef SIM_PHYSX_PHYSICS_H
#define SIM_PHYSX_PHYSICS_H

#include <PxPhysicsAPI.h>

class Physics {

public:
    static Physics &get() {
        static Physics instance;
        return instance;
    }

    static auto get_physics() {
        return Physics::get().physics;
    }

    /** @brief Set number of CPU used for all scenes computation. */
    static void set_num_cpu(int num_cpu) {
        Physics::get().dispatcher = physx::PxDefaultCpuDispatcherCreate(num_cpu);
    }

    Physics(Physics const &) = delete;

    void operator=(Physics const &) = delete;

    virtual ~Physics() {
#define SAFE_RELEASE(x)    if(x)    { x->release(); x = nullptr;    }
        release_all_scenes();
        SAFE_RELEASE(dispatcher);
        SAFE_RELEASE(cooking);
        SAFE_RELEASE(physics);
        SAFE_RELEASE(foundation);
    }

private:
    Physics() {
        using namespace physx;
        foundation = PxCreateFoundation(PX_PHYSICS_VERSION, allocator, error_callback);
        physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale());
        cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, PxCookingParams(PxTolerancesScale()));
        dispatcher = physx::PxDefaultCpuDispatcherCreate(0);
    }

    void release_all_scenes() {
        std::vector<physx::PxScene *> scenes(physics->getNbScenes());
        physics->getScenes(&scenes[0], scenes.size());
        for (auto &scene : scenes) {
            scene->release();
        }
    }

public:
    physx::PxDefaultAllocator allocator;
    physx::PxDefaultErrorCallback error_callback;
    physx::PxFoundation *foundation = nullptr;
    physx::PxPhysics *physics = nullptr;
    physx::PxCooking *cooking = nullptr;

    physx::PxDefaultCpuDispatcher *dispatcher = nullptr;

};

#endif //SIM_PHYSX_PHYSICS_H