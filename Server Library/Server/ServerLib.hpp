#ifndef _SERVER_LIBRARY_NOSNET_HPP_
#define _SERVER_LIBRARY_NOSNET_HPP_

#include <boost/asio.hpp>

#include <NosStdLib/DynamicArray.hpp>

namespace ServerLib
{
	namespace ClientManagement
	{
		class ClientTracker
		{
		public:
			/// <summary>
			/// Emun which is used to define the clients last known status
			/// </summary>
			enum ClientStatus : UINT8
			{
				Offline = 0,
				Online = 1,
			};
		private:
			static inline NosStdLib::DynamicArray<ClientTracker*> ClientArray; /* Array of all clients that have joined */

			std::wstring ClientUsername; /* Client's name which they choose */
			ClientStatus ClientCurrentStatus; /* Client's status on if they are online, offline and etc */
			boost::asio::ip::tcp::socket* SessionConnectionSocket; /* Session's ConnectionSocket to get the endpoint from */

			/// <summary>
			/// Constructor
			/// </summary>
			ClientTracker(const std::wstring& clientUsername, const ClientStatus& clientCurrentStatus, boost::asio::ip::tcp::socket* sessionConnectionSocket)
			{
				ClientUsername = clientUsername;
				ClientCurrentStatus = clientCurrentStatus;
				SessionConnectionSocket = sessionConnectionSocket;

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
}
#endif