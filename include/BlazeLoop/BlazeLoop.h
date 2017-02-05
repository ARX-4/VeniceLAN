#pragma once

#include <WS2tcpip.h>

void BlazeLoop_Init();

hostent* WINAPI BlazeLoop_GetHostByName(const char* name);

int WINAPI BlazeLoop_Connect(SOCKET socket, const sockaddr* name, int namelen);

int WINAPI BlazeLoop_Send(SOCKET socket, const char* buf, int len, int flags);

int WINAPI BlazeLoop_SendTo(SOCKET socket, const char* buf, int len, int flags, const sockaddr* to, int tolen);

int WINAPI BlazeLoop_Recv(SOCKET socket, char* buf, int len, int flags);

int WINAPI BlazeLoop_RecvFrom(SOCKET socket, char* buf, int len, int flags, sockaddr* from, int* fromlen);

int WINAPI BlazeLoop_Select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, const timeval* timeout);

int WINAPI BlazeLoop_CloseSocket(SOCKET socket);

#include "BlazeServer.h"
#include "HubServer.h"
#include "AppServer.h"