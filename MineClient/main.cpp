#include "Globals.h"
#include "Client.h"
#include "Protocol.h"
#include "MineEngine.h"
#include "Inputs.h"
#include "InputHandler.h"
#include "lib/glm/glm.hpp"

using namespace MineClient::Network;

 int main(int argc, char* argv[])
{
	Client*			client;
	CMineEngine		engine;
	vox::Inputs*	inputs;
	id::Obj			steve;
	std::string		debugOculusStr;
	bool running = true;
	bool debugOculus = false;
	std::cout << "W/A/S/D to move " << std::endl;
	std::cout << "Q/E to dig and put blocks " << std::endl;
	std::cout << "Plug the Oculus Rift to use it " << std::endl << std::endl;
	client = new (std::nothrow) Client();
	if (client == nullptr)
		return -1;
	client->ConnectToServer();
	std::cout << "Debug Oculus Rift ? (Y/N) : ";
	std::getline(std::cin, debugOculusStr);
	if (debugOculusStr == "Y" || debugOculusStr == "y")
		debugOculus = true;
	if (!engine.Init(debugOculus))
		return -1;
	if (!steve.loadObj("Steve.obj.model"))
		return -1;
	inputs = engine.GetInputs();
	while (running && client->IsRunning())
	{
		bool	shouldClose;
		if (!engine.Update(shouldClose))
		{
			client->Quit();
			client->Destroy();
			engine.Destroy();
			delete client;
			return -1;
		}
		if (shouldClose)
			break;
		Protocol::HandleReadBuffer(*client, engine, &steve);
		InputHandler::HandleInputs(inputs, &client, engine);
	}
	client->Quit();
	client->Destroy();
	engine.Destroy();
	delete client;
	return 0;
}
