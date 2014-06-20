#include "stdafx.h"
#include "FPSCamera.h"


vox::FPSCamera::FPSCamera(GLFWwindow* window, int windowWidth, int windowHeight)
{
	_window = window;
	_windowWidth  = windowWidth;
	_windowHeight = windowHeight;
	_windowMidX = _windowWidth  / 2;
	_windowMidY = _windowHeight / 2;

	glfwSetCursorPos(_window, _windowMidX, _windowMidY);

	_pos = glm::vec3(0.0f, 900.0f, 0.0f);
	_rot = glm::quat();
	_speed = glm::vec3(0.0f);

	
	_speedFactor = 30.0f;

	_pitchSensitivity = 0.2f;
	_yawSensitivity   = 0.2f;

	_holdingForward		= false;
	_holdingBackward	= false;
	_holdingLeftStrafe	= false;
	_holdingRightStrafe	= false;
	_jumping			= false;
}


vox::FPSCamera::~FPSCamera(void)
{
}

void
vox::FPSCamera::handleMouseMove(int mouseX, int mouseY, bool moved)
{

	glfwSetCursorPos(_window, _windowMidX, _windowMidY);
}

void 
vox::FPSCamera::move(float deltaTime, std::map<vox::Vector3Int, vox::Chunk*> chunkMap)
{
}