#ifndef _SERVER_CONNECTIONPROCESSING_NOSNET_HPP_
#define _SERVER_CONNECTIONPROCESSING_NOSNET_HPP_

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

#include <Central/CentralLib.hpp>
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

		void UserClientPath(boost::asio::ip::tcp::socket* connectionSocket, CentralLib::ClientManagement::ClientTracker* currentConnectionClientTracker)
		{
			currentConnectionClientTracker = AliasedClientTracker::RegisterClient(L"Client Username", CentralLib::ClientInterfacing::StrippedClientTracker::ClientStatus::Client, connectionSocket);

            wprintf(CentralLib::ClientInterfacing::StrippedClientTracker::ListClientArray().c_str());

			/* Tell client that it is ready for next step */
			AliasedServerResponse::CreateSerializeSend(connectionSocket, AliasedServerResponse::InformationCodes::Ready, L"Server is ready for next step");

			{
				/* Wait for client to be ready */
				boost::asio::streambuf serverReponseBuffer;
				boost::asio::read_until((*connectionSocket), serverReponseBuffer, Definition::Delimiter);
				CentralLib::Communications::CentralizedClientResponse clientReponse(&serverReponseBuffer);

				if (clientReponse.GetInformationCode() != CentralLib::Communications::CentralizedClientResponse::InformationCodes::Ready)
				{
					CentralLib::Logging::LogMessage<wchar_t>(L"Client sent unexpected response messages, escaping\n", true);
					return;
				}
			}

			boost::asio::streambuf streamBuffer;
			CentralLib::ClientInterfacing::StrippedClientTracker::SerializeArray(&streamBuffer, *(currentConnectionClientTracker->GetArrayPositionPointer()));

            CentralLib::Write(connectionSocket, streamBuffer);
		}

		void UserHostPath(boost::asio::ip::tcp::socket* connectionSocket, AliasedClientTracker* currentConnectionClientTracker)
		{
            currentConnectionClientTracker = AliasedClientTracker::RegisterClient(L"SERVER", CentralLib::ClientInterfacing::StrippedClientTracker::ClientStatus::Hosting, connectionSocket);
		}
	}
}

#endif