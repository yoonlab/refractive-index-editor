#include "PhysicsInterface.h"

PhysicsInterface::PhysicsInterface()
{
	config = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(config);
	broadphase = new btDbvtBroadphase();
	world = new btCollisionWorld(dispatcher, broadphase, config);
}

PhysicsInterface::~PhysicsInterface()
{

}

void PhysicsInterface::addCollisionObject(btCollisionObject *obj)
{
	world->addCollisionObject(obj);
}

void PhysicsInterface::addCollisionObjectsFromScene(Scene *scene)
{
	for (auto node : scene->sceneNodes)
	{
		addCollisionObject(node.collisionObject);
	}
}

btVector3 PhysicsInterface::rayPick(btVector3 &orig, btVector3 &dir)
{
	btVector3 end = orig + dir * rayDist;
	btCollisionWorld::ClosestRayResultCallback rayCallback(orig, end);
	world->rayTest(orig, end, rayCallback);

	if (rayCallback.hasHit())
	{
		return rayCallback.m_hitPointWorld;
	}
	return orig;
}
