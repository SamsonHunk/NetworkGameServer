#include "SimulationObj.h"

SimulationObj::SimulationObj(b2World* worldIn)
{
	physicsWorld = worldIn;
}

SimulationObj::~SimulationObj()
{
}

void SimulationObj::deleteThis()
{
	needsDestroyed = true;
}

bool SimulationObj::getDelete()
{
	return needsDestroyed;
}

b2Body * SimulationObj::getPhysicsBody()
{
	return physicsBody;
}
