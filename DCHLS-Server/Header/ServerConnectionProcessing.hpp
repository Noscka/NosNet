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
		void UserClientPath(boost::asio::ip::tcp::socket* connectionSocket, CentralLib::ClientManagement::ClientTracker* currentConnectionClientTracker)
		{
            bool initialValidation = true;
            while (initialValidation)
            { /* Scoped to delete usernameBuffer after use */
                /* Get Username from Client */
                boost::asio::streambuf usernameBuffer;
                size_t lenght = boost::asio::read_until((*connectionSocket), usernameBuffer, Definition::Delimiter);

                std::wstring clientsUsername = CentralLib::streamBufferToWstring(&usernameBuffer, lenght);

                boost::asio::streambuf responseBuffer;

                if (CentralLib::Validation::ValidateUsername(clientsUsername)) /* username is valid */
                {
                    /* Create ClientTracker Object and attach it to current session */
                    currentConnectionClientTracker = CentralLib::ClientManagement::ClientTracker::RegisterClient(clientsUsername, CentralLib::ClientInterfacing::StrippedClientTracker::ClientStatus::Client, connectionSocket);
                    initialValidation = false;
                    ServerLib::Communications::ServerResponse(CentralLib::Communications::CentralizedServerResponse::InformationCodes::Accepted, L"server accepted username").serializeObject(&responseBuffer);
                }
                else /* username isn't valid */
                {
                    ServerLib::Communications::ServerResponse(CentralLib::Communications::CentralizedServerResponse::InformationCodes::NotAccepted, L"server didn't accept username").serializeObject(&responseBuffer);
                }

                boost::asio::write((*connectionSocket), responseBuffer);
                boost::asio::write((*connectionSocket), boost::asio::buffer(Definition::Delimiter));
            }

            wprintf(CentralLib::ClientInterfacing::StrippedClientTracker::ListClientArray().c_str());

			/* Wait for client to be ready */
			boost::asio::streambuf serverReponseBuffer;
			boost::asio::read_until((*connectionSocket), serverReponseBuffer, Definition::Delimiter);
			CentralLib::Communications::CentralizedClientResponse clientReponse(&serverReponseBuffer);

            if (clientReponse.GetInformationCode() != CentralLib::Communications::CentralizedClientResponse::InformationCodes::Ready)
            {
                CentralLib::Logging::LogMessage<wchar_t>(L"Client sent unexpected response messages, escaping\n", true);
                return;
            }

			boost::asio::streambuf streamBuffer;
			CentralLib::ClientInterfacing::StrippedClientTracker::SerializeArray(&streamBuffer, *(currentConnectionClientTracker->GetArrayPositionPointer()));
			boost::asio::write((*connectionSocket), streamBuffer);
			boost::asio::write((*connectionSocket), boost::asio::buffer(Definition::Delimiter));
		}

		void UserHostPath(boost::asio::ip::tcp::socket* connectionSocket, CentralLib::ClientManagement::ClientTracker* currentConnectionClientTracker)
		{
            currentConnectionClientTracker = CentralLib::ClientManagement::ClientTracker::RegisterClient(L"SERVER", CentralLib::ClientInterfacing::StrippedClientTracker::ClientStatus::Hosting, connectionSocket);
		}
	}
}

#endif