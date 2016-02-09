#pragma once

#include <btBulletCollisionCommon.h>
#include "Scene.h"

class PhysicsInterface
{
public:
	PhysicsInterface();
	virtual ~PhysicsInterface();

	void addCollisionObject(btCollisionObject *obj);
	void addCollisionObjectsFromScene(Scene *scene);
	btVector3 rayPick(btVector3 &orig, btVector3 &dir);

	static btCollisionObject *createCollisionObjectFromVertices(const std::vector<Vertex> *vertexData);

private:
	btCollisionConfiguration *config;
	btCollisionDispatcher *dispatcher;
	btBroadphaseInterface *broadphase;
	btCollisionWorld *world;

	const float rayDist = 1000.0f;
};