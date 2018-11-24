#pragma once
#include "SimulationObj.h"
class Floor : public SimulationObj
{
public:
	using SimulationObj::SimulationObj;
	void update();
	void init();
};