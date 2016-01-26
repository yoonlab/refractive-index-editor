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

void PhysicsInterface::addCollisionShape(btCollisionObject *obj)
{
	world->addCollisionObject(obj);
}