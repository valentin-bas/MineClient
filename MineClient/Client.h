#pragma once

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#include "ByteBuffer.h"
#include <mutex>

namespace std
{
	class thread;
}

#define SIZE_BUF 2048

namespace MineClient
{
	namespace Network
	{
		enum States
		{
			DISCONNECTED,
			CONNECT,
			LOGGING,
			GAME
		};

		class Client
		{
		public:
			Client(void);
			~Client(void);

			void	ConnectToServer(void);

			bool	Init(const AString& name, const AString& server,
						 const AString& port);
			bool	Destroy();

			cByteBuffer*	TryLockReadBuf();
			cByteBuffer*	LockReadBuf();
			void			ReleaseReadBuf();
			void			WriteToSendBuf(const char* data, unsigned int len);
			AString&		GetServer() { return _server; }
			AString&		GetPort() { return _port; }
			AString&		GetName() { return _name; }
			Int32			GetState() const { return _state; }
			void			SetState(Int32 state) { _state = state; }

			inline void		Quit() { _running = false; }
			inline bool		IsRunning() { return _running; }

		private:
			SOCKET	_socket;
			bool	_initialized;
			Int32	_state;

			std::mutex	_mutexRead;
			cByteBuffer	_readBuf;
			std::mutex	_mutexWrite;
			cByteBuffer	_writeBuf;
			AString		_server;
			AString		_port;
			AString		_name;
			
			void	_Update();
			bool	_Recv();
			bool	_Send();

			static bool _InitWinSock();
			static void _DestroyWinSock();

			static Int32	_winSockInitialized;
			volatile bool	_running;
			std::thread*	_thread;
		};
	}
}
