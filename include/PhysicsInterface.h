#pragma once

#include <btBulletCollisionCommon.h>

class PhysicsInterface
{
public:
	PhysicsInterface();
	virtual ~PhysicsInterface();

	void addCollisionShape(btCollisionObject *obj);
private:
	btCollisionConfiguration *config;
	btCollisionDispatcher *dispatcher;
	btBroadphaseInterface *broadphase;
	btCollisionWorld *world;
};