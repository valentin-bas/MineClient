#pragma once

#include <mutex>
#include <queue>

namespace vox
{
	class ChunkColumn;
}

namespace std
{
	class thread;
}

namespace vox
{
	class Chunk;
	class ChunkLoader
	{
	public:
		struct ChunkToLoad
		{
			int		x;
			int		y;
			int		z;
			void*	data;
			int		compressSize;
		};

		ChunkLoader(void);
		~ChunkLoader(void);

		bool	Init(void);
		void	Run(void);

		bool	AddChunkToLoad(int x, int y, int z, void* data, int compressSize);
		Chunk*	GetLoadedChunks(void);
		void	Destroy();


		//Inline
		inline void Quit(void) { _running = false; }
	private:

		volatile bool		_running;
		std::thread*		_thread;

		std::queue<ChunkToLoad*>	_chunkToLoad;
		std::mutex					_chunkToLoadMutex;
		std::queue<Chunk*>			_chunkLoaded;
		std::mutex					_chunkLoadedMutex;

		ChunkToLoad*	_GetChunkToLoad();
		bool			_LoadChunk(ChunkToLoad* toLoad);
	};
}