#pragma once

namespace vox
{
	class Chunk;
	class FPSCamera
	{
		glm::vec3	_pos;
		glm::quat	_rot;

		glm::vec3	_speed;

		float		_speedFactor;
		float		_pitchSensitivity;
		float		_yawSensitivity;

		int			_windowWidth;
		int			_windowHeight;
		int			_windowMidX;
		int			_windowMidY;
		GLFWwindow*	_window;

		bool		_holdingForward;
		bool		_holdingBackward;
		bool		_holdingLeftStrafe;
		bool		_holdingRightStrafe;
		bool		_jumping;

	public:
		FPSCamera(GLFWwindow* window, int theWindowWidth, int theWindowHeight);
		~FPSCamera(void);

		void handleMouseMove(int mouseX, int mouseY, bool moved);
		void move(float deltaTime, std::map<vox::Vector3Int, Chunk*> chunkMap);
	};
};
