#pragma once

#include "MineEngine.h"
#include "Inputs.h"
#include "Client.h"

class InputHandler
{
public:
	static void HandleInputs(vox::Inputs* inputs, MineClient::Network::Client** client, CMineEngine& engine);
private:
	InputHandler(void);
	~InputHandler(void);
};

