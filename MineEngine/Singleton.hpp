/// Singleton.h: interface for the Singleton class.
///
/// This idea is taken from Scott Bilas 
/// [GameProgrammingGems copy write 2000 Charles River Media, inc.]
///////////////////////////////////////////////////////////////////////

#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include <cassert>

/// Allows for one instance of a class - Can decide when created and when destroyed.
template <typename T> 
class Singleton  
{
private:
	static T* ms_Singleton;

public:
	Singleton()
	{
		assert(!ms_Singleton && "Pointer to ms_Singleton is NULL");
		int offset = (int)(T*)1 - (int)(Singleton <T>*)(T*)1;
		ms_Singleton = (T*)((int)this + offset);
	}

	~Singleton()
	{
		assert(ms_Singleton && "Nothing to destroy");
		ms_Singleton = 0;
	}

	static T& GetSingleton()
	{
		assert(ms_Singleton && "Singleton not initialized correctly");
		return (*ms_Singleton);
	}

	static T* GetSingletonPtr()
	{
		return (ms_Singleton);
	}
};

template <typename T> 
T* Singleton <T>::ms_Singleton = 0;

#endif