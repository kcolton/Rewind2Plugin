#pragma once
#include "bakkesmod/plugin/bakkesmodplugin.h"

class EntityState
{
public:
	EntityState();
	EntityState(Vector setLocation, Vector setVelocity, Rotator setRotation, Vector setAngularVelocity);
	~EntityState();

	Vector location;
	Vector velocity;
	Rotator rotation;
	Vector angularVelocity;
};