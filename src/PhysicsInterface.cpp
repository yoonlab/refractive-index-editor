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
        addCollisionObject(node->getCollisionObjectPtr());
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

btCollisionObject * PhysicsInterface::createCollisionObjectFromVertices(const std::vector<Vertex> *vertexData)
{
    btTriangleMesh *triMesh = new btTriangleMesh();
    for (int k = 0; k < vertexData->size() / 3; k += 3)
    {
        btVector3 position[3] =
        {
            btVector3(vertexData->at(k).position[0], vertexData->at(k).position[1], vertexData->at(k).position[2]),
            btVector3(vertexData->at(k + 1).position[0], vertexData->at(k + 1).position[1], vertexData->at(k + 1).position[2]),
            btVector3(vertexData->at(k + 2).position[0], vertexData->at(k + 2).position[1], vertexData->at(k + 2).position[2])
        };
        triMesh->addTriangle(position[0], position[1], position[2]);
    }
    btBvhTriangleMeshShape *triColShape = new btBvhTriangleMeshShape(triMesh, true);
    btCollisionObject *colObj = new btCollisionObject();
    colObj->setCollisionShape(triColShape);
    return colObj;
}
