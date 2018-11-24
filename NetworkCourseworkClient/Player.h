#pragma once
#include "SimulationObj.h"
class Player : public SimulationObj
{
public:
	using SimulationObj::SimulationObj;
	void update();
	void init();

	
};