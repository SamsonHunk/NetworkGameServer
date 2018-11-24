#pragma once
#include "Box2D.h"

//base physics object class for simulating movement

class SimulationObj
{
public:
	SimulationObj(b2World* worldIn);
	~SimulationObj();

	void deleteThis(); //when iterating through a container to delete something, set this to true
	bool getDelete();
	b2Body* getPhysicsBody();

	//virtual functions
	virtual void update() = 0;
	virtual void init() = 0;
protected:
	b2World* physicsWorld;
	bool needsDestroyed = false;
	b2Body* physicsBody;
};
