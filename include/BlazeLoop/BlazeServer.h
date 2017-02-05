#pragma once

#include <queue>

namespace BlazeLoop
{
class Component
{
private:
	virtual void HandleRequest() = 0;
};

class BlazeServer
{
private:
	std::unordered_map<uint16_t, Component*> m_components;

private:
	std::queue<std::string> m_incomingQueue;

	std::queue<uint8_t> m_outgoingQueue;

private:
	CRITICAL_SECTION m_inCritSec;

	CRITICAL_SECTION m_outCritSec;

public:
	BlazeServer();

public:
	virtual const char* GetHostName() = 0;

public:
	bool IsPacketAvailable();

public:
	void RegisterComponent(uint16_t componentID, Component* component);

	void HandleRequest();
};
}