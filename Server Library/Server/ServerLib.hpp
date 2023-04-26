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
			friend boost::serialization::access;

			template<class Archive>
			void serialize(Archive& archive, const unsigned int version)
			{
				for (ClientTracker* entry : ClientArray)
				{
					archive& entry->ClientUsername;
					archive& entry->ClientCurrentStatus;

					std::wstring ipString = NosStdLib::String::ConvertString<wchar_t, char>(entry->SessionConnectionSocket->remote_endpoint().address().to_v4().to_string());

					archive& ipString; /* TODO: Figure out direct serialization */
				}
			}

			template<class Archive>
			void deserialize(Archive& archive, const unsigned int version)
			{
			}

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

			void serializeObject(std::streambuf* Streambuf)
			{
				boost::archive::binary_oarchive oa(*Streambuf);
				oa&* (this);
			}

			void DeserializeObject(boost::asio::streambuf* Streambuf)
			{
				boost::archive::binary_iarchive ia(*Streambuf);
				ia&* (this);
			}
		};

	}

	namespace Communications
	{
	}
}
#endif