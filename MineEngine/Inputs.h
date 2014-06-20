#pragma once
#ifdef MINEENGINE_EXPORTS
#define MINEENGINE_API __declspec(dllexport)
#else
#define MINEENGINE_API __declspec(dllimport)
#endif

namespace vox
{
	class MINEENGINE_API Inputs
	{	
		bool*	_keys;
		bool*	_keysRepeat;
		bool*	_mouseKey;
		bool	_mouseMoved;
		int*	_mouseMove;
		float	_oculusYaw;
		float	_oculusPitch;
		float	_oculusRoll;
	public:
	
		Inputs(void);
		~Inputs(void);
	
		void HandleKeyDown(char key);
		void HandleKeyRepeat(char key);
		void HandleKeyUp(char key);
		void HandleMouseMove(int x, int y);
		void HandleOculusOrientation(float yaw, float pitch, float roll);
	
		bool GetKeyDown(char key);
		bool GetKeyRepeat(char key);
		void GetOculusOrientation(float& yaw, float& pitch, float& roll);
		int* GetMouseMove(void);
		bool IsMouseMoved(void);
		void resetMouseMove(void);
		void resetKeyDown(void);
	};
	
}