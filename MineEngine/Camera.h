#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <map>
#include "Vector3Int.h"
#include "ChunkColumn.h"


class Camera
{
	protected:
		// Camera position
		glm::vec3 position;

		// Camera rotation
		glm::vec3 rotation;

		// Camera movement speed. When we call the move() function on a camera, it moves using these speeds
		glm::vec3 speed;

		bool isGravity;

		float movementSpeedFactor; // Controls how fast the camera moves
		float pitchSensitivity;    // Controls how sensitive mouse movements affect looking up and down
		float yawSensitivity;      // Controls how sensitive mouse movements affect looking left and right

		// Window size in pixels and where the midpoint of it falls
		int windowWidth;
		int windowHeight;
		int windowMidX;
		int windowMidY;

		GLFWwindow*	_window;

		bool		_isOculus;

		// Method to set some reasonable default values. For internal use by the class only.
		void initCamera();

	public:
		static const float TO_RADS; // The value of 1 degree in radians

		// Holding any keys down?
		bool holdingForward;
		bool holdingBackward;
		bool holdingLeftStrafe;
		bool holdingRightStrafe;
		bool jumping;

		// Constructors
		Camera(GLFWwindow* window, int windowWidth, int windowHeight);

		// Destructor
		~Camera();

		// Mouse movement handler to look around
		void handleMouseMove(int mouseX, int mouseY, bool moved);

		// Method to convert an angle in degress to radians
		float toRads(const float &angleInDegrees) const;

		// Method to move the camera based on the current direction
		void move(float deltaTime, std::map<vox::Vector3Int, vox::Chunk*> chunkMap);
		bool collide(glm::vec3 pos, std::map<vox::Vector3Int, vox::Chunk*> chunkMap);
		void replaceCam(std::map<vox::Vector3Int, vox::Chunk*> chunkMap);
		void faceCollision(glm::vec3& pos, glm::vec3& ret, std::map<vox::Vector3Int, vox::Chunk*> chunkMap);

		void ChangeGravity(void);
		void EnableOculus(void);

		// --------------------------------- Inline methods ----------------------------------------------

		// Setters to allow for change of vertical (pitch) and horizontal (yaw) mouse movement sensitivity
		float getPitchSensitivity()            { return pitchSensitivity;  }
		void  setPitchSensitivity(float value) { pitchSensitivity = value; }
		float getYawSensitivity()              { return yawSensitivity;    }
		void  setYawSensitivity(float value)   { yawSensitivity   = value; }

		// Position getters
		void  getPosition(glm::vec3& pos) { pos = position; }
		float getXPos()           const { return position.x; }
		float getYPos()           const { return position.y; }
		float getZPos()           const { return position.z; }

		// Rotation getters
		glm::vec3 getRotation() const { return rotation;        }
		float getXRot()           const { return rotation.x; }
		float getYRot()           const { return rotation.y; }
		float getZRot()           const { return rotation.z; }
};

#endif // CAMERA_H
