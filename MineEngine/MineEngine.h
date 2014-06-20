#pragma once
#ifdef MINEENGINE_EXPORTS
#define MINEENGINE_API __declspec(dllexport)
#else
#define MINEENGINE_API __declspec(dllimport)
#endif

#include "lib\glm\glm.hpp"
#include "Obj.h"

namespace vox
{
	class Core;	
	class Inputs;
	class ChunkLoader;
}

class Camera;

// Cette classe est exportée de MineEngine.dll
class MINEENGINE_API CMineEngine
{
public:
	enum KeysMoves
	{
		FORWARD,
		BACKWARD,
		STRAFE_LEFT,
		STRAFE_RIGHT,
		JUMP
	};

	CMineEngine(void);

	bool			Init(bool debugOculus);
	bool			Update(bool& shouldClose);
	bool			Destroy(void);
	bool			SwitchToFullScreen(void);
	bool			LoadChunk(int x, int y, int z, unsigned char* data, int compressSize);
	bool			UnloadChunk(int x, int y, int z);
	void			GetPlayerPosition(glm::vec3& pos, glm::vec3& rot);
	vox::Inputs*	GetInputs(void);
	void			UpdateBlockChange(int x, int y, int z, int id);
	void			UpdateCamera(bool* moves);
	int				LoadEntityFromObj(id::Obj& obj, int askedId = -1);
	void			GetEntityTransform(int id, glm::vec3& pos, glm::vec3& rot); 
	void			SetEntityTransform(int id, const glm::vec3& pos, const glm::vec3& rot);
	void			HandleMouseMove(int* moves, bool moved);
	void			PlayerBlockInteraction(glm::vec4& block, glm::vec3& cursor, bool build = false, int id = 0);
	void			ChangeGravity(void);
private:
	vox::Core*			_core;
	vox::ChunkLoader*	_chunkLoader;
	Camera*				_camera;

	bool				_running;
};