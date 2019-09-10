#include "EntityState.h"

EntityState::EntityState() :
	location(Vector(0, 0, 0)), velocity(Vector(0, 0, 0)), rotation(Rotator(0, 0, 0)), angularVelocity(Vector(0, 0, 0))
{
}

EntityState::EntityState(Vector setLocation, Vector setVelocity, Rotator setRotation, Vector setAngularVelocity) :
	location(setLocation), velocity(setVelocity), rotation(setRotation), angularVelocity(setAngularVelocity)
{
}

EntityState::~EntityState()
{
}
