#pragma once

namespace BlazeLoop
{
class HubServer : public BlazeServer
{
public:
	HubServer();

	virtual const char* GetHostName() override;
};

class RedirectorComponent : public Component
{
public:
	RedirectorComponent();

	virtual void HandleRequest() override;
};
}