#ifndef _CLIENT_RUNTIME_NOSNET_HPP_
#define _CLIENT_RUNTIME_NOSNET_HPP_

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

#include <Central/CentralLib.hpp>

namespace ClientLib
{
	namespace Runtime
	{
		/// <summary>
		/// Function ran if User chose to be a client
		/// </summary>
		/// <param name="connectionSocket">- Pointer to connection socket</param>
		void NormalClient(boost::asio::io_context* io_context, boost::asio::ip::tcp::socket* connectionSocket) /* Rename to something more descriptive */
		{
			/* Once sent username, tell server client is ready for Array */
			boost::asio::streambuf responseBuffer;
			ClientLib::Communications::ClientResponse(CentralLib::Communications::CentralizedClientResponse::InformationCodes::Ready, L"Client is ready for array").serializeObject(&responseBuffer);
			CentralLib::Write(connectionSocket, responseBuffer);

			/* Receive array */
			boost::asio::streambuf ContentBuffer;
			boost::asio::read_until((*connectionSocket), ContentBuffer, Definition::Delimiter);
			CentralLib::ClientInterfacing::StrippedClientTracker::DeserializeArray(&ContentBuffer);

			/* List the array */
			wprintf(CentralLib::ClientInterfacing::StrippedClientTracker::ListClientArray().c_str());

			/* Disconnect from DCHLS server */
			(*connectionSocket).cancel();

			/* TEMP: Get first IP in array (guaranteed to be a server since only Client Server ips get sent) */
			std::string ipAddress = (*CentralLib::ClientInterfacing::StrippedClientTracker::GetClientArray())[0]->ReturnIPAddress();

			/*
			Connect to the Client Server (DNICC not DCHLS)
			*/
			boost::asio::connect((*connectionSocket), boost::asio::ip::tcp::resolver((*io_context)).resolve(ipAddress, Constants::DefaultClientHostPort));
			CentralLib::Logging::LogMessage<wchar_t>(L"Connected to server\n", true);

			ClientLib::StartUp::GatherUsername(connectionSocket);

			while (true)
			{
				/* Basic chat implementation just for now to test the communications capabilities */
				std::string message;

				std::getline(std::cin, message);

				CentralLib::Write(connectionSocket, boost::asio::buffer(message));
			}
		}
	}
}

#endif