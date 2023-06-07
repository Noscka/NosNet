#ifndef _CLIENT_RUNTIME_NOSNET_HPP_
#define _CLIENT_RUNTIME_NOSNET_HPP_

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

#include <Central/CentralLib.hpp>

namespace ClientLib
{
	/* Aliased with using StrippedClientTracker */
	using AliasedSCT = CentralLib::ClientInterfacing::StrippedClientTracker;

	namespace Functions
	{
		/// <summary>
		/// Returns a IP Address of a server that is chosen by the user
		/// </summary>
		/// <returns>IP Address of a server</returns>
		std::string ChooseServer()
		{
			/* List all clients */
			AliasedSCT::ListClientArray();

			bool choosing = true;
			int serverIndex = -1;
			std::string connectionChoiceString;

			while (choosing)
			{
				(void)wprintf(L"Enter Server Index: "); /* TEMP NUMBER INPUT */

				std::getline(std::cin, connectionChoiceString);

				if (sscanf_s(connectionChoiceString.c_str(), "%d", &serverIndex) != 1)
				{ /* Conversion failed */
					(void)wprintf(L"Invalid argument, please input again\n");
					continue;
				}

				if (serverIndex < 0 || serverIndex > AliasedSCT::GetClientArray()->GetArrayIndexPointer()-1) /* TODO: Validate range */
				{ /* out of range */
					(void)wprintf(L"input was out of range\n");
					continue;
				}

				choosing = false; /* Past all checks, is valid */
			}

			return (*AliasedSCT::GetClientArray())[serverIndex]->ReturnIPAddress();
		}
	}

	namespace Runtime
	{
		/// <summary>
		/// Function ran if User chose to be a client
		/// </summary>
		/// <param name="connectionSocket">- Pointer to connection socket</param>
		void NormalClient(boost::asio::io_context* io_context, boost::asio::ip::tcp::socket* connectionSocket) /* Rename to something more descriptive */
		{
			/* Tell server which path going down */
			ClientLib::Communications::ClientResponse::CreateSerializeSend(connectionSocket, CentralLib::Communications::CentralizedClientResponse::InformationCodes::GoingClientPath, L"User is Client");

			/* Once sent username, tell server client is ready for Array */
			ClientLib::Communications::ClientResponse::CreateSerializeSend(connectionSocket, CentralLib::Communications::CentralizedClientResponse::InformationCodes::Ready, L"Client is ready for array");

			/* Receive array */
			boost::asio::streambuf ContentBuffer;
			boost::asio::read_until((*connectionSocket), ContentBuffer, Definition::Delimiter);
			AliasedSCT::DeserializeArray(&ContentBuffer);

			/* List the array */
			(void)wprintf(AliasedSCT::ListClientArray().c_str());

			/* Disconnect from DCHLS server */
			(*connectionSocket).cancel();

			/* TEMP: put into variable and then use */
			std::string ipAddress = ClientLib::Functions::ChooseServer();

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