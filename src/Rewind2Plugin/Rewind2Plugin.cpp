#include "Rewind2Plugin.h"

BAKKESMOD_PLUGIN(Rewind2Plugin, "Rewind2 Plugin", "0.1", PLUGINTYPE_FREEPLAY)

// frames to record. at least 10 seconds at 250 fps
#define REWIND_HISTORY_LENGTH 2500

#define CVAR_REWIND_ENABLED "rewind2_enabled"
#define CVAR_REWIND_BUTTON "rewind2_button"
#define CVAR_DEBUG_ENABLED "rewind2_debug_enabled"


void Rewind2Plugin::onLoad()
{	
	cvarManager->log("Rewind2Plugin loaded");

	gameWrapper->HookEventWithCaller<CarWrapper>("Function TAGame.Car_TA.SetVehicleInput", std::bind(&Rewind2Plugin::OnSetInput, this, std::placeholders::_1, std::placeholders::_2));
	gameWrapper->HookEvent("Function PlayerController_TA.Driving.PlayerMove", std::bind(&Rewind2Plugin::OnPlayerMove, this));

	cvarManager->registerCvar(CVAR_REWIND_ENABLED, "1", "Enable rewind", true, true, 0.f, true, 1.f);
	
	cvarManager->registerCvar(CVAR_REWIND_BUTTON, "XboxTypeS_LeftThumbStick", "Button for rewind controls").addOnValueChanged([this](std::string old, CVarWrapper now) {
		rewindKey = this->gameWrapper->GetFNameIndexByString(now.getStringValue());
		cvarManager->log("REWIND_BUTTON cvar:" + now.getStringValue() + " key:" + to_string(rewindKey));
	});
	cvarManager->getCvar(CVAR_REWIND_BUTTON).notify(); // store rewindKey on first load

	cvarManager->registerCvar(CVAR_DEBUG_ENABLED, "0", "Enable debug output", true, true, 0.f, true, 1.f);
}

void Rewind2Plugin::onUnload()
{
	cvarManager->log("Rewind2Plugin unloaded");
}


void Rewind2Plugin::OnSetInput(CarWrapper cw, void * params)
{
	if (!gameWrapper->IsInFreeplay()) {
		return;
	}

	ServerWrapper training = gameWrapper->GetGameEventAsServer();
	BallWrapper ball = training.GetBall();

	ControllerInput* ci = (ControllerInput*)params;

	if (cvarManager->getCvar(CVAR_REWIND_ENABLED).getBoolValue() && gameWrapper->IsKeyPressed(rewindKey)) {
		ci->Roll = 0;
		ci->Throttle = 0;
		ci->Pitch = 0;
		ci->Yaw = 0;
		ci->DodgeForward = 0;
		ci->DodgeStrafe = 0;
		ci->Handbrake = 0;
		ci->HoldingBoost = 0;
		ci->ActivateBoost = 0;
		ci->Jump = 0;
	}
}

void Rewind2Plugin::OnPlayerMove()
{
	if (!gameWrapper->IsInFreeplay()) {
		return;
	}

	if (cvarManager->getCvar(CVAR_DEBUG_ENABLED).getBoolValue()) {
		cvarManager->log("OnPlayerMove");
	}

	if (!cvarManager->getCvar(CVAR_REWIND_ENABLED).getBoolValue()) {
		return;
	}

	ServerWrapper server = gameWrapper->GetGameEventAsServer();
	BallWrapper ball = server.GetBall();
	CarWrapper car = server.GetCars().Get(0);

	if (gameWrapper->IsKeyPressed(rewindKey)) {
		// Pop the last game state off stack and restore
		if (!rewindHistory.empty()) {
			GameState state = rewindHistory.back();
			rewindHistory.pop_back();

			if (cvarManager->getCvar(CVAR_DEBUG_ENABLED).getBoolValue()) {
				cvarManager->log("Replaying history");
			}

			//car.SetLocation(state.car.location + Vector(0,0,1));
			//car.SetRotation(state.car.rotation);
			car.SetVelocity(Vector(0, 0, 0) - state.car.velocity);
			car.SetAngularVelocity(Vector(0, 0, 0) - state.car.angularVelocity, false);

			ball.SetLocation(state.ball.location);
			ball.SetRotation(state.ball.rotation);
			ball.SetVelocity(Vector(0, 0, 0));
			ball.SetAngularVelocity(Vector(0, 0, 0), false);

			car.GetBoostComponent().SetCurrentBoostAmount(state.boostAmount);

			wasRewinding = true;
			playBackState = state;
		}
		else {
			//cvarManager->log("Empty history, nothing to replay");
		}
	}
	else if (wasRewinding) {
		// restore the playback state
		car.SetLocation(playBackState.car.location);
		car.SetVelocity(playBackState.car.velocity);
		car.SetRotation(playBackState.car.rotation);
		car.SetAngularVelocity(playBackState.car.angularVelocity, false);

		ball.SetLocation(playBackState.ball.location);
		ball.SetVelocity(playBackState.ball.velocity);
		ball.SetRotation(playBackState.ball.rotation);
		ball.SetAngularVelocity(playBackState.ball.angularVelocity, false);

		car.GetBoostComponent().SetCurrentBoostAmount(playBackState.boostAmount);

		wasRewinding = false;
	}
	else {
		// Push current game state to rewind history
		if (rewindHistory.size() >= REWIND_HISTORY_LENGTH) {
			int numToRemove = 1 + rewindHistory.size() - REWIND_HISTORY_LENGTH;
			rewindHistory.erase(rewindHistory.begin(), rewindHistory.begin() + numToRemove);
		}

		GameState state = GameState(
			EntityState(
				car.GetLocation(),
				car.GetVelocity(),
				car.GetRotation(),
				car.GetAngularVelocity()
			),
			EntityState(
				ball.GetLocation(),
				ball.GetVelocity(),
				ball.GetRotation(),
				ball.GetAngularVelocity()
			),
			car.GetBoostComponent().IsNull() ? 0 : car.GetBoostComponent().GetCurrentBoostAmount()
		);

		rewindHistory.push_back(state);

		// cvarManager->log("Recording history - length:" + to_string(rewindHistory.size()));
		wasRewinding = false;
	}

}