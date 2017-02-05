#include "StdInc.h"
#include "HubServer.h"

namespace BlazeLoop
{
HubServer::HubServer()
{
	RegisterComponent(5, new RedirectorComponent());
}

const char* HubServer::GetHostName()
{
	return "gosredirector.ea.com";
}

RedirectorComponent::RedirectorComponent()
{

}

void RedirectorComponent::HandleRequest()
{
	printf("Handling RedirectorComponent request...\n");
}
}