#pragma once
#include "EntityState.h"

class GameState
{
public:
	GameState();
	GameState(EntityState setCar, EntityState setBall, float setBoostAmount);
	~GameState();

	EntityState car;
	EntityState ball;
	float boostAmount;
};
