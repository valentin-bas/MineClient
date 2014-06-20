#include "Globals.h"
#include "InputHandler.h"
#include "Protocol.h"

using namespace MineClient::Network;

InputHandler::InputHandler(void)
{
}


InputHandler::~InputHandler(void)
{
}

void InputHandler::HandleInputs(vox::Inputs* inputs, MineClient::Network::Client** client, CMineEngine& engine)
{
	bool	CameraMoves[5], Moved = false;

	ZeroMemory(CameraMoves, sizeof(CameraMoves));
	bool mmoved = false;
	if (inputs->IsMouseMoved())
		mmoved = true;
	//{
		engine.HandleMouseMove(inputs->GetMouseMove(), mmoved);
		inputs->resetMouseMove();
		Moved = true;
	//}
	if (inputs->GetKeyRepeat('W'))
		CameraMoves[CMineEngine::KeysMoves::FORWARD] = true;		Moved = true;
	if (inputs->GetKeyRepeat('S'))
		CameraMoves[CMineEngine::KeysMoves::BACKWARD] = true;		Moved = true;
	if (inputs->GetKeyRepeat('A'))
		CameraMoves[CMineEngine::KeysMoves::STRAFE_LEFT] = true;	Moved = true;
	if (inputs->GetKeyRepeat('D'))
		CameraMoves[CMineEngine::KeysMoves::STRAFE_RIGHT] = true;	Moved = true;
	if (inputs->GetKeyDown(' '))
		CameraMoves[CMineEngine::KeysMoves::JUMP] = true;			Moved = true;
	engine.UpdateCamera(CameraMoves);
	if (inputs->GetKeyDown('Q'))
	{
		glm::vec4	block;
		glm::vec3	cursor;

		engine.PlayerBlockInteraction(block, cursor);
		if (block.y >= 0)
			Protocol::SendDig(**client, 0, block);
	}
	if (inputs->GetKeyDown('E'))
	{
		glm::vec4	block;
		glm::vec3	cursor;

		engine.PlayerBlockInteraction(block, cursor, true, 1);
		if (block.y >= 0)
			Protocol::SendBlockPlacment(**client, block, cursor);
	}
	if (inputs->GetKeyDown('G'))
	{
		engine.ChangeGravity();
	}
	//if (inputs->GetKeyDown('T'))
	//{
	//	Client*	newClient = new (std::nothrow) Client();
	//	if (client == nullptr)
	//		return; // false
	//	if (newClient->Init("Stranger", "192.168.0.47", "1338"))
	//	{
	//		(*client)->Destroy();
	//		delete *client;
	//		*client = newClient;
	//	}
	//	else
	//		delete newClient;
	//}
	if (Moved)
	{
		glm::vec3 posPlayer;
		glm::vec3 rotPlayer;
		engine.GetPlayerPosition(posPlayer, rotPlayer);
		Protocol::SendPlayerPosition(**client, posPlayer.x, posPlayer.y, posPlayer.z,
									 rotPlayer.x, rotPlayer.y);
	}
	inputs->resetKeyDown();
}