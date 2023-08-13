#pragma once

#include "ClientEntry.hpp"

namespace ClientLib
{
	class ClientManager : public ClientLib::ClientEntry
	{
	private:
		friend boost::serialization::access;
		template<class Archive>
		void serialize(Archive& archive, const unsigned int version)
		{
			// serialize base class information
			archive& boost::serialization::base_object<ClientLib::ClientEntry>(*this);
		}

		boost::asio::ip::tcp::socket* SessionConnectionSocket; /* Session's ConnectionSocket to get the endpoint from */

		/// <summary>
		/// Constructor
		/// </summary>
		ClientManager(const std::wstring& clientName, const enClientStatus& clientStatus, boost::asio::ip::tcp::socket* sessionConnectionSocket)
		{
			ClientName = clientName;
			ClientStatus = clientStatus;
			SessionConnectionSocket = sessionConnectionSocket;

			boost::asio::ip::tcp::endpoint* tempPointer = new boost::asio::ip::tcp::endpoint(sessionConnectionSocket->remote_endpoint());
			TargetEndpoint = tempPointer;

			ClientRegistry.Append(this);
		}

	public:

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
}