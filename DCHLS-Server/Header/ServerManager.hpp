#pragma once
#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>

#include <NosLib/String.hpp>
#include <NosLib/Cast.hpp>
#include <NosLib/DynamicArray.hpp>

#include <Central/ServerEntry.hpp>

#include <string.h>
#include <format>
#include <iostream>
#include <fstream>

class ServerManager : public Central::ServerEntry
{
private:
	friend boost::serialization::access;
	template<class Archive>
	void serialize(Archive& archive, const unsigned int version)
	{
		// serialize base class information
		archive& boost::serialization::base_object<Central::ServerEntry>(*this);
	}

	boost::asio::ip::tcp::socket* SessionConnectionSocket; /* Session's ConnectionSocket to get the endpoint from */

	/// <summary>
	/// Constructor
	/// </summary>
	ServerManager(const std::wstring& serverName, const enServerType& serverType, const enServerStatus& serverStatus, boost::asio::ip::tcp::socket* sessionConnectionSocket)
	{
		ServerName = serverName;
		ServerType = serverType;
		ServerStatus = serverStatus;
		SessionConnectionSocket = sessionConnectionSocket;

		boost::asio::ip::tcp::endpoint* tempPointer = new boost::asio::ip::tcp::endpoint(sessionConnectionSocket->remote_endpoint());
		TargetEndpoint = tempPointer;

		ServerRegistry.Append(this);
	}

public:

	boost::asio::ip::tcp::socket* GetConnectionSocket()
	{
		return SessionConnectionSocket;
	}

	/// <summary>
	/// Changes clients status
	/// </summary>
	void ChangeStatus(const enServerStatus& newServerStatus)
	{
		ServerStatus = newServerStatus;
	}

	/// <summary>
	/// Static contructor as the created object *NEED* to be created with the "new" keyword to stay alive
	/// </summary>
	/// <param name="serverName">- Server's name</param>
	/// <param name="serverType">- Server's type</param>
	/// <param name="serverStatus">- Server's current status</param>
	/// <param name="sessionConnectionSocket">- pointer to the current connection socket</param>
	/// <returns>pointer to created object</returns>
	static ServerManager* RegisterServer(const std::wstring& serverName, const enServerType& serverType, const enServerStatus& serverStatus, boost::asio::ip::tcp::socket* sessionConnectionSocket)
	{
		return new ServerManager(serverName, serverType, serverStatus, sessionConnectionSocket);
	}
};