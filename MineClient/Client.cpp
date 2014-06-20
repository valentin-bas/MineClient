#include "Globals.h"
#include "Client.h"
#include "Protocol.h"

#include <thread>

#define DEBUG_SERVER "10.0.3.19"
#define DEBUG_PORT "1337"
#define DEBUG_NAME "Bot"

using namespace MineClient::Network;

int	Client::_winSockInitialized = 0;

Client::Client(void) :
	_socket(INVALID_SOCKET), _initialized(false),
	_readBuf(SIZE_BUF), _writeBuf(SIZE_BUF), _state(States::DISCONNECTED),
	_thread(nullptr), _running(false)
{
}


Client::~Client(void)
{
}

void	Client::ConnectToServer(void)
{
	std::string serverAddr;
	std::string serverPort;
	std::string login;
	bool connected = true;
	do
	{
		#ifdef _DEBUG
		if (connected == true)
		{
			serverAddr = DEBUG_SERVER;
			serverPort = DEBUG_PORT;
			login = DEBUG_NAME;
			std::cout << "Enter the server address: " + serverAddr << std::endl;
			std::cout << "Enter the server port: " + serverPort << std::endl;
			std::cout << "Enter your Login: " + login << std::endl;
		}
		else
		#endif
		{
			std::cout << "Enter the server address: ";
			std::getline(std::cin, serverAddr);
			std::cout << "Enter the server port: ";
			std::getline(std::cin, serverPort);
			std::cout << "Enter your Login: ";
			std::getline(std::cin, login);
		}
		if (!Init(login, serverAddr, serverPort))
			connected = false;
		else
			connected = true;
	} while (connected == false);
}

bool	Client::Init(const AString& name, const AString& server,
					 const AString& port)
{
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	int iResult;

	_name = name;
	_server = server;
	_port = port;
	if (!_InitWinSock())
		return false;
	_initialized = true;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	iResult = getaddrinfo(server.c_str(), port.c_str(), &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		Destroy();
		return false;
	}
	for(ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (_socket == INVALID_SOCKET)
		{
			printf("socket failed with error: %ld\n", WSAGetLastError());
			Destroy();
			return false;
		}
		iResult = connect(_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(_socket);
			_socket = INVALID_SOCKET;
			continue;
		}
		_state = States::CONNECT;
		break;
	}
	freeaddrinfo(result);
	if (_socket == INVALID_SOCKET)
	{
		printf("Unable to connect to server!\n");
		Destroy();
		return false;
	}
	Protocol::SendHandshake(*this);
	Protocol::SendLoginStart(*this);
	_running = true;
	_thread = new (std::nothrow) std::thread(&Client::_Update, this);
	if (_thread == nullptr)
		return false;
	printf("connected !\n");
	return true;
}

void	Client::_Update(void)
{
	fd_set	readSet;
	fd_set	writeSet;
	int		iResult;

	while (_running)
	{
		if (_initialized == false)
			return;
		FD_ZERO(&readSet);
		FD_ZERO(&writeSet);
		FD_SET(_socket, &readSet);
		if (_writeBuf.CanReadBytes(1))
			FD_SET(_socket, &writeSet);
		iResult = select(1, &readSet, &writeSet, nullptr, nullptr);
		if (iResult == SOCKET_ERROR)
		{
			printf("socket failed with error: %ld\n", WSAGetLastError());
			return;
		}
		else if (iResult == 0)
			continue;
		if (FD_ISSET(_socket, &readSet) && _readBuf.CanWriteBytes(512))
		{
			if (_Recv() == false)
			{
				Quit();
				return;
			}
		}
		if (FD_ISSET(_socket, &writeSet))
		{
			if (_Send() == false)
			{
				Quit();
				return;
			}
		}
	}
	_running = false;
}

bool	Client::Destroy(void)
{
	if (_thread != nullptr)
	{
		Quit();
		_thread->join();
		delete _thread;
	}
	_thread = nullptr;
	if (_initialized == false)
		return false;
	_initialized = false;
	if (_winSockInitialized > 0 && _socket != INVALID_SOCKET)
		if (closesocket(_socket) != 0)
		{
			printf("shutdown failed with error: %d\n", WSAGetLastError());
			return false;
		}
		_DestroyWinSock();
		return true;
}

bool	Client::_Recv(void)
{
	char tmpBuf[512];
	int byteRead;

	std::lock_guard<std::mutex> locker(_mutexRead);
	byteRead = recv(_socket, tmpBuf, 512, 0);
	if ( byteRead > 0 )
		;//printf("Bytes received: %d\n", byteRead);
	else if (byteRead == 0)
	{
		printf("Connection closed\n");
		return false;
	}
	else
	{
		printf("recv failed with error: %d\n", WSAGetLastError());
		return false;
	}
	if (!_readBuf.Write(tmpBuf, byteRead))
		return false;
	return true;
}

bool	Client::_Send(void)
{
	AString	data;

	std::lock_guard<std::mutex> locker(_mutexWrite);
	_writeBuf.ReadAll(data);
	_writeBuf.CommitRead();
	if (send(_socket, data.data(), data.length(), 0) == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		return false;
	}
	return true;
}

cByteBuffer*
Client::TryLockReadBuf()
{
	if (_mutexRead.try_lock())
		return &_readBuf;
	else
		return nullptr;
}

cByteBuffer*
Client::LockReadBuf()
{
	_mutexRead.lock();
	return &_readBuf;
}

void
Client::ReleaseReadBuf()
{
	_mutexRead.unlock();
}

void
Client::WriteToSendBuf(const char* data, unsigned int len)
{
	_mutexWrite.lock();
	_writeBuf.Write(data, len);
	_mutexWrite.unlock();
}

bool	Client::_InitWinSock(void)
{
	int result;
	WSADATA wsaData;
	int sem;

	sem = _winSockInitialized;
	++_winSockInitialized;
	if (sem > 0)
		return true;
	result = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (result != 0)
	{
		printf("WSAStartup failed with error: %d\n", result);
		_winSockInitialized = 0;
		return false;
	}
	return true;
}

void	Client::_DestroyWinSock(void)
{
	--_winSockInitialized;
	if (_winSockInitialized == 0)
		WSACleanup();
}