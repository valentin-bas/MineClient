// MineEngine.cpp : définit les fonctions exportées pour l'application DLL.
//

#include "stdafx.h"
#include "MineEngine.h"
#include "Core.h"
#include "Camera.h"
#include "ChunkLoader.h"
#include "Chunk.h"

CMineEngine::CMineEngine() :
	_core(nullptr), _chunkLoader(nullptr),
	_running(false)
{
	return;
}

bool
CMineEngine::Init(bool debugOculus)
{
	_core = new (std::nothrow) vox::Core();
	if (_core == nullptr)
		return false;
	_chunkLoader = new (std::nothrow) vox::ChunkLoader();
	if (_chunkLoader == nullptr)
		return false;
	if (!_chunkLoader->Init())
		return false;
	if (!_core->init(debugOculus))
		return false;
	_camera = _core->GetCamera();
	_running = true;
	return true;
}

bool
CMineEngine::Update(bool& shouldClose)
{
	if (_running == false)
		return false;
	
	for (int i = 0; i < 3; ++i)
	{
		vox::Chunk*	loaded = _chunkLoader->GetLoadedChunks();
		if (loaded != nullptr)
		{
			if (!_core->AddLoadedChunk(loaded))
			{
				delete loaded;
				return false;
			}
		}
	}
	if (!_core->run(shouldClose))
		return false;
	return true;
}

bool
CMineEngine::Destroy(void)
{
	_running = false;
	_chunkLoader->Quit();
	_chunkLoader->Destroy();
	if (_chunkLoader != nullptr)
		delete _chunkLoader;
	if (_core != nullptr)
		delete _core;
	return true;
}

bool
CMineEngine::SwitchToFullScreen(void)
{
	return _core->SwitchToFullScreen();
}

bool
CMineEngine::LoadChunk(int x, int y, int z, unsigned char* data, int compressSize)
{
	if (!_chunkLoader->AddChunkToLoad(x, y, z, data, compressSize))
		return false;
	return true;
}

bool
CMineEngine::UnloadChunk(int x, int y, int z)
{
	if (!_core->UnLoadChunk(vox::Vector3Int(x, y, z)))
		return false;
	return true;
}

void
CMineEngine::GetPlayerPosition(glm::vec3& pos, glm::vec3& rot)
{
	_core->GetPosPlayer(pos, rot);
}

void
CMineEngine::UpdateBlockChange(int x, int y, int z, int id)
{
	_core->UpdateBlockChange(x, y, z, id);
}

///<summary>return the id of the entity or -1 if failed
///can ask to use a specific if. If this id is already used, return -2</summary>
int
CMineEngine::LoadEntityFromObj(id::Obj& obj, int askedId)
{
	return _core->LoadEntityFromObj(obj, askedId);
}

void
CMineEngine::GetEntityTransform(int id, glm::vec3& pos, glm::vec3& rot)
{
	_core->GetEntityTransform(id, pos, rot);
}

void
CMineEngine::SetEntityTransform(int id, const glm::vec3& pos, const glm::vec3& rot)
{
	_core->SetEntityTransform(id, pos, rot);
}

vox::Inputs*
CMineEngine::GetInputs(void)
{
	return _core->GetInputs();
}

void
CMineEngine::HandleMouseMove(int* moves, bool moved)
{
	_camera->handleMouseMove(moves[0], moves[1], moved);
}

void
CMineEngine::UpdateCamera(bool* moves)
{
	_camera->holdingBackward = moves[KeysMoves::BACKWARD];
	_camera->holdingForward = moves[KeysMoves::FORWARD];
	_camera->holdingLeftStrafe = moves[KeysMoves::STRAFE_LEFT];
	_camera->holdingRightStrafe = moves[KeysMoves::STRAFE_RIGHT];
	_camera->jumping = moves[KeysMoves::JUMP];
}

void
CMineEngine::PlayerBlockInteraction(glm::vec4& block, glm::vec3& cursor, bool build, int id)
{
	_core->selectBlock(block, cursor, build, id);
}

void
CMineEngine::ChangeGravity(void)
{
	_camera->ChangeGravity();
}