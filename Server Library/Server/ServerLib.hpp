#ifndef _SERVER_LIBRARY_NOSNET_HPP_
#define _SERVER_LIBRARY_NOSNET_HPP_

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>
#include <string.h>
#include <string>

#include <boost/asio.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>

#include <NosStdLib/DynamicArray.hpp>
#include <NosStdLib/String.hpp>

#include <Central/CentralLib.hpp>

namespace ServerLib
{
	namespace ClientManagement
	{
		class ClientTracker : public CentralLib::ClientInterfacing::StrippedClientTracker
		{
		private:
			friend boost::serialization::access;
			template<class Archive>
			void serialize(Archive& archive, const unsigned int version)
			{
				// serialize base class information
				archive& boost::serialization::base_object<CentralLib::ClientInterfacing::StrippedClientTracker>(*this);
			}

			//static inline NosStdLib::DynamicArray<ClientTracker*> ClientArray; /* Array of all clients that have joined */

			boost::asio::ip::tcp::socket* SessionConnectionSocket; /* Session's ConnectionSocket to get the endpoint from */

			/// <summary>
			/// Constructor
			/// </summary>
			ClientTracker(const std::wstring& clientUsername, const ClientStatus& clientCurrentStatus, boost::asio::ip::tcp::socket* sessionConnectionSocket)
			{
				ClientUsername = clientUsername;
				ClientCurrentStatus = clientCurrentStatus;
				SessionConnectionSocket = sessionConnectionSocket;

				boost::asio::ip::tcp::endpoint* tempPointer = new boost::asio::ip::tcp::endpoint(sessionConnectionSocket->remote_endpoint());
				TargetEndpoint = tempPointer;

				ClientArray.Append(this);
			}

		public:
			/// <summary>
			/// Static contructor as the created object *NEED* to be created with the "new" keyword to stay alive
			/// </summary>
			/// <param name="clientUsername">- The client defined username</param>
			/// <param name="clientCurrentStatus">- the clients status when created</param>
			/// <param name="sessionConnectionSocket">- pointer to the clients endpoint (remote_endpoint)</param>
			/// <returns>pointer to self</returns>
			static ClientTracker* RegisterClient(const std::wstring& clientUsername, const ClientStatus& clientCurrentStatus, boost::asio::ip::tcp::socket* sessionConnectionSocket)
			{
				return new ClientTracker(clientUsername, clientCurrentStatus, sessionConnectionSocket);
			}
		};

	}

	namespace Communications
	{
		class ServerResponse
		{

		};
	}
}
#endif