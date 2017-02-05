#include "StdInc.h"
#include "BlazeLoop.h"

#include <unordered_map>
#include <process.h>

using namespace BlazeLoop;

static std::unordered_map<std::string, BlazeServer*> _servers;
static std::unordered_map<int, BlazeServer*> _serverHashes;
static std::map<SOCKET, BlazeServer*> _sockets;

void BlazeLoop_RegisterServer(BlazeServer* server)
{
	printf("[BlazeLoop] registering server %s\n", server->GetHostName());

	_servers[server->GetHostName()] = server;
}

hostent* WINAPI BlazeLoop_GetHostByName(const char* name)
{
	std::unordered_map<std::string, BlazeServer*>::const_iterator i = _servers.find(name);

	if (i != _servers.end())
	{
		printf("[BlazeLoop] gethostbyname(): found server %s in server list\n", name);

		static in_addr blazeAddr;
		blazeAddr.s_addr = 0x7F000000 | (std::hash<std::string>()(name) & 0xFFFFFF);

		static in_addr* blazeSockAddrList[2];
		blazeSockAddrList[0] = &blazeAddr;
		blazeSockAddrList[1] = nullptr;

		static hostent blazeHostEnt;
		blazeHostEnt.h_name = const_cast<char*>(name);
		blazeHostEnt.h_aliases = NULL;
		blazeHostEnt.h_addrtype = AF_INET;
		blazeHostEnt.h_length = sizeof(in_addr);
		blazeHostEnt.h_addr_list = (char**)blazeSockAddrList;

		_serverHashes[blazeAddr.s_addr] = i->second;

		return &blazeHostEnt;
	}

	return gethostbyname(name);
}

int WINAPI BlazeLoop_Connect(SOCKET socket, const sockaddr* name, int namelen)
{
	if (namelen == sizeof(sockaddr_in))
	{
		sockaddr_in* addr = (sockaddr_in*)name;

		std::unordered_map<int, BlazeServer*>::const_iterator i = _serverHashes.find(addr->sin_addr.s_addr);

		if (i != _serverHashes.end())
		{
			printf("[BlazeLoop] connect(): found server %s in server hashes\n", i->second->GetHostName());

			_sockets[socket] = i->second;

			return 0;
		}
	}

	return connect(socket, name, namelen);
}

static BlazeServer* BlazeLoop_LookupSocket(SOCKET socket)
{
	printf("[BlazeLoop] LookupSocket() called for socket %d\n", socket);

	std::map<SOCKET, BlazeServer*>::const_iterator i = _sockets.find(socket);

	if (i != _sockets.end())
	{
		printf("[BlazeLoop] LookupSocket(): found socket %d for server %s\n", socket, i->second->GetHostName());

		return i->second;
	}

	return nullptr;
}

int WINAPI BlazeLoop_Send(SOCKET socket, const char* buf, int len, int flags)
{
	BlazeServer* server = BlazeLoop_LookupSocket(socket);

	if (server)
	{
		printf("[BlazeLoop] send(): forwarding packet of length %d to server %s\n", len, server->GetHostName());

		//std::string packet(buf, len);
		//server->deferredHandlePacket(packet);

		return len;
	}

	return send(socket, buf, len, flags);
}

int WINAPI BlazeLoop_SendTo(SOCKET socket, const char* buf, int len, int flags, const sockaddr* to, int tolen)
{
	BlazeServer* server = BlazeLoop_LookupSocket(socket);

	if (server)
	{
		printf("[BlazeLoop] sendto(): forwarding packet of length %d to server %s\n", len, server->GetHostName());

		//std::string packet(buf, len);
		//server->deferredHandlePacket(packet);

		return len;
	}

	return sendto(socket, buf, len, flags, to, tolen);
}

int WINAPI BlazeLoop_Recv(SOCKET socket, char* buf, int len, int flags)
{
	BlazeServer* server = BlazeLoop_LookupSocket(socket);

	if (!server)
	{
		return recv(socket, buf, len, flags);
	}

	printf("[BlazeLoop] recv(): dequeueing packet of server %s\n", server->GetHostName());

	/*if (server->isPacketAvailable())
	{
		return server->dequeuePacket(buf, len);
	}*/

	WSASetLastError(WSAEWOULDBLOCK);
	return SOCKET_ERROR;
}

int WINAPI BlazeLoop_RecvFrom(SOCKET socket, char* buf, int len, int flags, sockaddr* from, int* fromlen)
{
	BlazeServer* server = BlazeLoop_LookupSocket(socket);

	if (!server)
	{
		return recvfrom(socket, buf, len, flags, from, fromlen);
	}

	printf("[BlazeLoop] recvfrom(): dequeueing packet of server %s\n", server->GetHostName());

	/*if (server->IsPacketAvailable())
	{
		return server->DequeuePacket(buf, len);
	}*/

	WSASetLastError(WSAEWOULDBLOCK);
	return SOCKET_ERROR;
}

int WINAPI BlazeLoop_Select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, const timeval* timeout)
{
	std::vector<SOCKET> readSockets;
	std::vector<SOCKET> writeSockets;

	for (std::map<SOCKET, BlazeServer*>::const_iterator i = _sockets.begin(); i != _sockets.end(); i++)
	{
		if (readfds)
		{
			if (FD_ISSET(i->first, readfds))
			{
				readSockets.push_back(i->first);

				FD_CLR(i->first, readfds);

				//printf("[BlazeLoop] removed socket from select() call (write)\n");
			}
		}

		if (writefds)
		{
			if (FD_ISSET(i->first, writefds))
			{
				writeSockets.push_back(i->first);

				//printf("[BlazeLoop] removed socket from select() call (read)\n");
			}
		}

		if (exceptfds)
		{
			if (FD_ISSET(i->first, exceptfds))
			{
				FD_CLR(i->first, exceptfds);

				//printf("[BlazeLoop] removed socket from select() call (except)\n");
			}
		}
	}

	int retval = select(nfds, readfds, writefds, exceptfds, timeout);

	for (int i = 0; i < readSockets.size(); i++)
	{
		FD_SET(readSockets.at(i), readfds);
		retval++;
	}

	for (int i = 0; i < writeSockets.size(); i++)
	{
		if (_sockets[writeSockets.at(i)]->IsPacketAvailable())
		{
			FD_SET(writeSockets.at(i), writefds);
			retval++;
		}
	}

	return retval;
}

int WINAPI BlazeLoop_CloseSocket(SOCKET socket)
{
	BlazeServer* server = BlazeLoop_LookupSocket(socket);

	if (server)
	{
		printf("[BlazeLoop] closesocket() erasing socket %d of server %s\n", socket, server->GetHostName()),

		_sockets.erase(socket);
		return 0;
	}

	return closesocket(socket);
}

void BlazeLoop_Init()
{
	BlazeLoop_RegisterServer(new HubServer());
	//BlazeLoop_RegisterServer(new AppServer());
}