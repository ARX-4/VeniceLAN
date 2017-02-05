#include "StdInc.h"
#include "BlazeServer.h"

namespace BlazeLoop
{
BlazeServer::BlazeServer()
{
	InitializeCriticalSection(&m_inCritSec);
	InitializeCriticalSection(&m_outCritSec);
}

bool BlazeServer::IsPacketAvailable()
{
	return !m_outgoingQueue.empty();
}

void BlazeServer::RegisterComponent(uint16_t componentID, Component* component)
{
	m_components.insert(std::make_pair(componentID, component));
}

void BlazeServer::HandleRequest()
{
	
}
}