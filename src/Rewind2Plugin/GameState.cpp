#include "GameState.h"


GameState::GameState()
{
}

GameState::GameState(EntityState setCar, EntityState setBall, float setBoostAmount)
	: car(setCar), ball(setBall), boostAmount(setBoostAmount)
{

}

GameState::~GameState()
{
}
