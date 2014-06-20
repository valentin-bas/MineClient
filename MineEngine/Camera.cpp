#include "stdafx.h"
#include "Camera.h"
#include "Chunk.h"
#include "glm/gtx/fast_square_root.hpp"
#include "Core.h"
#include "Inputs.h"

const float Camera::TO_RADS = 3.141592654f / 180.0f; // The value of 1 degree in radians

Camera::Camera(GLFWwindow* window, int theWindowWidth, int theWindowHeight) :
	_isOculus(false)
{
	_window = window;
	initCamera();

	windowWidth  = theWindowWidth;
	windowHeight = theWindowHeight;

	// Calculate the middle of the window
	windowMidX = windowWidth  / 2;
	windowMidY = windowHeight / 2;

	glfwSetCursorPos(_window, windowMidX, windowMidY);
}

Camera::~Camera()
{
	// Nothing to do here - we don't need to free memory as all member variables
	// were declared on the stack.
}

void Camera::initCamera()
{
	// Set position, rotation and speed values to zero
	position = glm::vec3(0.0f, 900.0f, 0.0f);
	rotation = glm::vec3(0.0f);
	speed = glm::vec3(0.0f);

	isGravity = false;

	// How fast we move (higher values mean we move and strafe faster)
	movementSpeedFactor = 30.0f;

	pitchSensitivity = 0.2f; // How sensitive mouse movements affect looking up and down
	yawSensitivity   = 0.2f; // How sensitive mouse movements affect looking left and right

	// To begin with, we aren't holding down any keys
	holdingForward     = false;
	holdingBackward    = false;
	holdingLeftStrafe  = false;
	holdingRightStrafe = false;
}

// Function to convert degrees to radians
float Camera::toRads(const float &theAngleInDegrees) const
{
	return theAngleInDegrees * TO_RADS;
}

// Function to deal with mouse position changes
void Camera::handleMouseMove(int mouseX, int mouseY, bool moved)
{
	// Calculate our horizontal and vertical mouse movement from middle of the window
	float horizMovement = (mouseX - windowMidX+1) * yawSensitivity;
	float vertMovement  = (mouseY - windowMidY) * pitchSensitivity;
	float yaw;
	float pitch;
	float roll;
	static float oldYaw = 0.0f;

	vox::Core::GetSingleton().GetInputs()->GetOculusOrientation(yaw, pitch, roll);

	if (_isOculus)
	{
		rotation.x = (float)((-pitch * 180) / M_PI);
		rotation.y += (float)(((oldYaw - yaw) * 180) / M_PI);
	}
	if (moved)
	{
		if (!_isOculus)
			rotation.x += (vertMovement);
		rotation.y += (horizMovement);
	}
	oldYaw = yaw;
	// Limit loking up to vertically up/down
	if (rotation.x < -90)
		rotation.x = -90;
	if (rotation.x > 90)
		rotation.x = 90;

	if (rotation.y < 0)
		rotation.y += 360;
	if (rotation.y > 360)
		rotation.y += -360;

	glfwSetCursorPos(_window, windowMidX, windowMidY);
}


// Function to calculate which direction we need to move the camera and by what amount
void Camera::move(float deltaTime, std::map<vox::Vector3Int, vox::Chunk*> chunkMap)
{
	// Vector to break up our movement into components along the X, Y and Z axis
	glm::vec3 movement = glm::vec3(0.0f);

	// Get the sine and cosine of our X and Y axis rotation
	float sinXRot = sinf( toRads( rotation.x) );
	float cosXRot = cosf( toRads( rotation.x) );

	float sinYRot = sinf( toRads( rotation.y) );
	float cosYRot = cosf( toRads( rotation.y) );

	float pitchLimitFactor = cosXRot; // This cancels out moving on the Z axis when we're looking up or down


	if (holdingForward)
	{
		movement.x += (sinYRot * pitchLimitFactor);
		movement.y += (-sinXRot);
		movement.z += (-cosYRot * pitchLimitFactor);
	}

	if (holdingBackward)
	{
		movement.x += (-sinYRot * pitchLimitFactor);
		movement.y += (sinXRot);
		movement.z += (cosYRot * pitchLimitFactor);
	}

	if (holdingLeftStrafe)
	{
		movement.x += (-cosYRot);
		movement.z += (-sinYRot);
	}

	if (holdingRightStrafe)
	{
		movement.x += (cosYRot);
		movement.z += (sinYRot);
	}

	glm::vec3 tmp = glm::fastNormalize(movement);
	movement = tmp;

	float framerateIndependentFactor = movementSpeedFactor * deltaTime;
	if (isGravity == true)
		speed -= 19.81f * deltaTime;

	movement *= framerateIndependentFactor;
	if (isGravity == true)
		movement.y = speed.y * deltaTime;
	position += movement;
	/*
	if (collide(movement + position + glm::vec3(0.0f, 0.0f, 0.0f), chunkMap) == false &&
		collide(movement + position + glm::vec3(0.0f, -3.2f, 0.0f), chunkMap) == false &&
		collide(movement + position + glm::vec3(-0.3f, -2.2f, 0.0f), chunkMap) == false &&
		collide(movement + position + glm::vec3(0.3f, -2.2f, 0.0f), chunkMap) == false &&
		collide(movement + position + glm::vec3(0.0f, -2.2f, -0.3f), chunkMap) == false &&
		collide(movement + position + glm::vec3(0.0f, -2.2f, 0.3f), chunkMap) == false &&
		collide(movement + position + glm::vec3(-0.3f, 0.0f, 0.0f), chunkMap) == false &&
		collide(movement + position + glm::vec3(0.3f, 0.0f, 0.0f), chunkMap) == false &&
		collide(movement + position + glm::vec3(0.0f, 0.0f, -0.3f), chunkMap) == false &&
		collide(movement + position + glm::vec3(0.0f, 0.0f, 0.3f), chunkMap) == false)
	{
		
	}
	else
	{
		replaceCam(chunkMap);
		if (jumping == true)
			speed.y = 15.0f;
		glm::vec3 ret;
		faceCollision(movement + position, ret, chunkMap);

		movement.y = glm::max(movement.y, 0.0f);
		position += movement * ret;
	}*/
}

//TODO REWRITE COLLISION CODE
bool Camera::collide(glm::vec3 pos, std::map<vox::Vector3Int, vox::Chunk*> chunkMap)
{/*
	pos.x = -pos.x;
	pos.z = -pos.z;
	vox::Vector2Int	key((int)glm::floor(pos.x / (float)(vox::Chunk::CHUNK_SIZE * 2)), (int)glm::floor(pos.z / (float)(vox::Chunk::CHUNK_SIZE * 2)));
	int xChunk = (int)glm::round(pos.x / 2.0f) - key.x * vox::Chunk::CHUNK_SIZE;
	int yChunk = (int)glm::round(pos.y / 2.0f);
	int zChunk = (int)glm::round(pos.z / 2.0f) - key.y * vox::Chunk::CHUNK_SIZE;
	int id;

	if (xChunk == 16)
	{
		++key.x;
		xChunk = 0;
	}
	if (zChunk == 16)
	{
		zChunk = 0;
		++key.y;
	}

	if (chunkMap.count(key) == 0)
		return false;
	id = chunkMap[key]->getBlockAt(xChunk, yChunk, zChunk);
	if (id == -1 || id == 0)
		return false;
	return true;*/
	return false;
}

void
Camera::replaceCam(std::map<vox::Vector3Int, vox::Chunk*> chunkMap)
{
	glm::vec3 posTmp = position;
	
	while (collide(posTmp, chunkMap) == true ||
		   collide(posTmp - glm::vec3(0.0f, 3.2f, 0.0f), chunkMap) == true)
		   posTmp.y += 2;
	position = posTmp;
}

void
Camera::faceCollision(glm::vec3& pos, glm::vec3& ret, std::map<vox::Vector3Int, vox::Chunk*> chunkMap)
{
	ret = glm::vec3(1.0f, 1.0f, 1.0f);
	if (collide(pos + glm::vec3(0.0f, 0.0f, 0.0f), chunkMap) == true)
	{
		ret.y = 0.0f;
	}
	if (collide(pos + glm::vec3(0.0f, -3.0f, 0.0f), chunkMap) == true )
	{
		ret.y = 0.0f;
		speed.y = 0.0f;
	}
	if (collide(pos + glm::vec3(-0.3f, -2.2f, 0.0f), chunkMap) == true ||
		collide(pos + glm::vec3(0.3f, -2.2f, 0.0f), chunkMap) == true  ||
		collide(pos + glm::vec3(-0.3f, 0.0f, 0.0f), chunkMap) == true ||
		collide(pos + glm::vec3(0.3f, 0.0f, 0.0f), chunkMap) == true )
	{
		ret.x = 0.0f;
		speed.x = 0.0f;
	}
	if (collide(pos + glm::vec3(0.0f, -2.2f, -0.3f), chunkMap) == true ||
		collide(pos + glm::vec3(0.0f, -2.2f, 0.3f), chunkMap) == true ||
		collide(pos + glm::vec3(0.0f, 0.0f, -0.3f), chunkMap) == true ||
		collide(pos + glm::vec3(0.0f, 0.0f, 0.3f), chunkMap) == true)
	{
		ret.z = 0.0f;
		speed.z = 0.0f;
	}
}

void
Camera::ChangeGravity(void)
{
	isGravity = !isGravity;
}

void
Camera::EnableOculus(void)
{
	_isOculus = true;
}