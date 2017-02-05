#pragma once

namespace BlazeLoop
{
class AppServer : public BlazeServer
{
public:
	AppServer();

	virtual const char* GetHostName() override;
};
}