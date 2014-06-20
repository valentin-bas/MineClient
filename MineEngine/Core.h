#ifndef	__CORE_H__
#define	__CORE_H__

#include "Camera.h"
#include "Vector3Int.h"
#include "Singleton.hpp"
#include "Inputs.h"
#include "Frustum.h"
#include <map>
#include <vector>
#include <OVR.h>

namespace id
{
	class Obj;
	class Model3;
}

class ParticleWorld;

namespace	vox
{
	class	Chunk;
	class	ChunkColumn;
	class	AbstractRenderer;
	class	TextRenderer;
	class	SkyDome;
	class	Entity;
	class	Core : public Singleton<Core>
	{
		private:
			Camera*		cam;
			SkyDome*	sky;
			GLFWwindow*	_window;
			int			_width;
			int			_height;
			float		_ratio;
			double		_oldTime;
			bool		_oculusPlug;

			//Input Management
			Inputs		_inputs;

			//World Management
			std::map<Vector3Int, Chunk*>	_chunkMap;
			std::map<int, Entity*>			_entities;

			AbstractRenderer*					_renderer;
			TextRenderer*						_textRenderer;

			//Raycast stuff
			glm::mat4	view;
			glm::mat4	projection;
			glm::vec3	right;
			glm::vec3	lookat;
			glm::vec3	position;
			glm::vec3	up;

			//Occulus
			OVR::HMDInfo						_info;
			OVR::SensorFusion*					_sensorFusion;

			//Debug
			bool	_debugOculus;

			//Particles
			ParticleWorld*	_particleWorld;

			Core(Core const& right);
			bool	__destroy(void);

			void	GetCursorBlockOffset(glm::vec4& offset, glm::vec3& chunkPos, glm::vec3& objCoord, Vector3Int& key);
			bool	Render(Frustum& frustrum, bool side);
		protected:

		public:
						Core(void);
			virtual		~Core(void);
			bool		init(bool debugOculus);
			bool		SwitchToFullScreen(int width = 0, int height = 0);
			bool		run(bool& shouldClose);

			void		updateVector(void);
			void		selectBlock(glm::vec4& res, glm::vec3& cursor, bool build, int id);
			float		fract(float value);

			static void	errorCallback(int error, const char* desc);
			static void	handleKeypress(GLFWwindow* window, int theKey, int scancode, int theAction, int mods);
			static void	handleMouseMove(GLFWwindow* window, double mouseX, double mouseY);

			int		LoadEntityFromObj(id::Obj& obj, int askedId);
			void	GetEntityTransform(int id, glm::vec3& pos, glm::vec3& rot); 
			void	SetEntityTransform(int id, const glm::vec3& pos, const glm::vec3& rot); 
			bool	AddLoadedChunk(Chunk* chunk);
			bool	UnLoadChunk(const Vector3Int& chunkPos);
			void	GetPosPlayer(glm::vec3& pos, glm::vec3& rot);
			
			void	UpdateBlockChange(int x, int y, int z, int id);

			ParticleWorld*	_CreateNewParticleWorld();

			//Inline
			Camera*	GetCamera(void) { return cam; }
			Inputs* GetInputs(void) { return &_inputs; }
	};
}

#endif
