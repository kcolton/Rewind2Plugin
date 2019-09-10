#pragma once
#pragma comment( lib, "bakkesmod.lib" )
#include "bakkesmod/plugin/bakkesmodplugin.h"

#include "GameState.h"
#include "EntityState.h"


class Rewind2Plugin : public BakkesMod::Plugin::BakkesModPlugin {
private:
	std::vector<GameState> rewindHistory;
	int rewindKey = 0;
	bool wasRewinding = false;
	GameState playBackState;

public:
	void onLoad() override;
	void onUnload() override;

	void OnSetInput(CarWrapper cw, void * params);
	void OnPlayerMove();
	void Render(CanvasWrapper canvas);
};