#ifndef _SERVER_CONNECTIONPROCESSING_NOSNET_HPP_
#define _SERVER_CONNECTIONPROCESSING_NOSNET_HPP_

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

#include <Central/CentralLib.hpp>
#include <Central/Communication.hpp>
#include <Central/Logging.hpp>
#include "ServerLib.hpp"

namespace ServerLib
{
	namespace Processing
	{
		namespace /* PRIVATE NAMESPACE */
		{
			/* Aliased with using StrippedClientTracker */
			using AliasedClientTracker = CentralLib::ClientManagement::ClientTracker;
			using AliasedServerResponse = ServerLib::Communications::ServerResponse;
		}

		inline void UserClientPath(boost::asio::ip::tcp::socket* connectionSocket, CentralLib::ClientManagement::ClientTracker*& currentConnectionClientTracker)
		{
			currentConnectionClientTracker = AliasedClientTracker::RegisterClient(L"Client Username", CentralLib::ClientInterfacing::StrippedClientTracker::UserStatus::Client, connectionSocket);

			/* Tell client that it is ready for next step */
			AliasedServerResponse::CreateSerializeSend(connectionSocket, AliasedServerResponse::InformationCodes::Ready, L"Server is ready for next step");
			CentralLib::Logging::CreateLog<wchar_t>(L"Sent Ready to user\n", false);

			{
				/* Wait for client to be ready */
				boost::asio::streambuf serverReponseBuffer;
				boost::asio::read_until((*connectionSocket), serverReponseBuffer, Definition::Delimiter);
				CentralLib::Communications::CentralizedClientResponse clientReponse(&serverReponseBuffer);

				if (clientReponse.GetInformationCode() != CentralLib::Communications::CentralizedClientResponse::InformationCodes::Ready)
				{
					CentralLib::Logging::CreateLog<wchar_t>(L"Client sent unexpected response messages, escaping\n", false);
					return;
				}
			}

			CentralLib::Logging::CreateLog<wchar_t>(L"Received Ready from user\n", false);

			boost::asio::streambuf streamBuffer;
			CentralLib::ClientInterfacing::StrippedClientTracker::SerializeArray(&streamBuffer, *(currentConnectionClientTracker->GetArrayPositionPointer()));

            CentralLib::Write(connectionSocket, streamBuffer);

			CentralLib::Logging::CreateLog<wchar_t>(L"Sent array\n", false);
		}

		inline void UserHostPath(boost::asio::ip::tcp::socket* connectionSocket, AliasedClientTracker*& currentConnectionClientTracker)
		{
            currentConnectionClientTracker = AliasedClientTracker::RegisterClient(L"SERVER", CentralLib::ClientInterfacing::StrippedClientTracker::UserStatus::Hosting, connectionSocket);
		}
	}
}

#endif