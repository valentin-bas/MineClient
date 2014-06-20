#include "stdafx.h"
#include "Inputs.h"
#include <OVR.h>

using namespace vox;

Inputs::Inputs(void)
{
	_keys = new bool[255];
	_keysRepeat = new bool[255];
	_mouseMove = new int[2];
	_mouseKey = new bool [3];
	_mouseMoved = false;
	ZeroMemory(_keys, sizeof(bool) * 255);
	ZeroMemory(_keysRepeat, sizeof(bool) * 255);
	ZeroMemory(_mouseKey, sizeof(bool) * 3);
	ZeroMemory(_mouseMove, sizeof(int) * 2);
}


Inputs::~Inputs(void)
{
	delete [] _keys;
	delete [] _keysRepeat;
	delete [] _mouseKey;
	delete [] _mouseMove;
}

void 
Inputs::HandleKeyDown(char key)
{
	_keys[key] = true;
	_keysRepeat[key] = true;
}

void
Inputs::HandleKeyRepeat(char key)
{
	_keys[key] = false;
}

void
Inputs::HandleKeyUp(char key)
{
	_keys[key] = false;
	_keysRepeat[key] = false;
}

void 
Inputs::HandleMouseMove(int x, int y)
{
	_mouseMove[0] = x;
	_mouseMove[1] = y;
	_mouseMoved = true;
}

void
Inputs::HandleOculusOrientation(float yaw, float pitch, float roll)
{
	_oculusYaw = yaw;
	_oculusPitch = pitch;
	_oculusRoll = roll;
}

bool
Inputs::GetKeyDown(char key)
{
	return _keys[key];
}

bool
Inputs::GetKeyRepeat(char key)
{
	return _keysRepeat[key];
}

void
Inputs::GetOculusOrientation(float& yaw, float& pitch, float& roll)
{
	yaw = _oculusYaw;
	pitch = _oculusPitch;
	roll = _oculusRoll;
}

int*
Inputs::GetMouseMove()
{
	return _mouseMove;
}

bool
Inputs::IsMouseMoved()
{
	return _mouseMoved;
}

void
Inputs::resetMouseMove()
{
	_mouseMoved = false;
}

void
Inputs::resetKeyDown()
{
	ZeroMemory(_keys, sizeof(bool) * 255);
}