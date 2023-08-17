#pragma once
#include <Central\Misc.hpp>
#include "ClientEntry.hpp"

class ClientManager : public ClientEntry
{
protected:
	friend boost::serialization::access;
	template<class Archive>
	void serialize(Archive& archive, const unsigned int version)
	{
		// serialize base class information
		archive& boost::serialization::base_object<ClientEntry>(*this);
	}

	boost::asio::ip::tcp::socket* SessionConnectionSocket; /* Session's ConnectionSocket to get the endpoint from */

	ClientManager(){}

	/// <summary>
	/// Constructor
	/// </summary>
	ClientManager(const std::wstring& clientName, const enClientStatus& clientStatus, boost::asio::ip::tcp::socket* sessionConnectionSocket)
	{
		ClientName = clientName;
		ClientStatus = clientStatus;
		SessionConnectionSocket = sessionConnectionSocket;

		ClientRegistry.Append(this);
	}

	~ClientManager()
	{
			
	}
public:

	boost::asio::ip::tcp::endpoint GetEndpoint()
	{
		return SessionConnectionSocket->remote_endpoint();
	}

	/// <summary>
	/// Get the current client's ip address as a string
	/// </summary>
	/// <returns>the ip address</returns>
	std::string GetIpAddressAsString()
	{
		return SessionConnectionSocket->remote_endpoint().address().to_v4().to_string();
	}

	boost::asio::ip::tcp::socket* GetConnectionSocket()
	{
		return SessionConnectionSocket;
	}

	/// <summary>
	/// Changes clients status
	/// </summary>
	void ChangeStatus(const enClientStatus& newClientStatus)
	{
		ClientStatus = newClientStatus;
	}

	/// <summary>
	/// Static contructor as the created object *NEED* to be created with the "new" keyword to stay alive
	/// </summary>
	/// <param name="clientName">- Client's name</param>
	/// <param name="clientStatus">- Client's type</param>
	/// <param name="sessionConnectionSocket">- pointer to the current connection socket</param>
	/// <returns>pointer to created object</returns>
	static ClientManager* RegisterClient(const std::wstring& clientName, const enClientStatus& clientStatus, boost::asio::ip::tcp::socket* sessionConnectionSocket)
	{
		return new ClientManager(clientName, clientStatus, sessionConnectionSocket);
	}
};

class SelfClient : public ClientManager
{
public:
	enum class enClientType : uint8_t
	{
		Client,
		Host,
	};
protected:
	friend boost::serialization::access;
	template<class Archive>
	void serialize(Archive& archive, const unsigned int version)
	{
		// serialize base class information
		archive& boost::serialization::base_object<ClientManager>(*this);
	}

	enClientType ClientType;

	SelfClient(const std::wstring& clientName, const enClientStatus& clientStatus, const enClientType& clientType)
	{
		ClientName = clientName;
		ClientStatus = clientStatus;
		ClientType = clientType;
	}

public:
	enClientType GetClientType()
	{
		return ClientType;
	}

	/// <summary>
	/// static constructor used to register self, acts different then RegisterClient
	/// </summary>
	/// <param name="clientName"></param>
	/// <param name="clientStatus"></param>
	/// <param name="clientType"></param>
	/// <returns></returns>
	static SelfClient* RegisterSelf(const std::wstring& clientName, const enClientStatus& clientStatus, const enClientType& clientType)
	{
		return new SelfClient(clientName, clientStatus, clientType);
	}
};