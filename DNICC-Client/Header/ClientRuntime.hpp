#ifndef _CLIENT_RUNTIME_NOSNET_HPP_
#define _CLIENT_RUNTIME_NOSNET_HPP_

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

namespace ClientLib
{
	namespace Runtime
	{
		/// <summary>
		/// Function ran if User chose to be a client
		/// </summary>
		/// <param name="connectionSocket">- Pointer to connection socket</param>
		void NormalClient(boost::asio::io_context* io_context, boost::asio::ip::tcp::socket* connectionSocket) /* Rename to something more describtive */
		{
			ClientLib::StartUp::GatherUsername(connectionSocket);

			boost::asio::streambuf ContentBuffer;
			boost::asio::read_until((*connectionSocket), ContentBuffer, Definition::Delimiter);

			CentralLib::ClientInterfacing::StrippedClientTracker::DeserializeArray(&ContentBuffer);

			wprintf(CentralLib::ClientInterfacing::StrippedClientTracker::ListClientArray().c_str());

			(*connectionSocket).cancel();


			std::string ipAddress = (*CentralLib::ClientInterfacing::StrippedClientTracker::GetClientArray())[0]->ReturnIPAddress();

			 /*
			Connects to the function using `resolver` which resolves the address e.g. (Noscka.com -> 123.123.123.123)
			Host - Hostname/Ip address
			Service - Service(Hostname for ports)/Port number
			*/
			boost::asio::connect((*connectionSocket), boost::asio::ip::tcp::resolver((*io_context)).resolve(ipAddress, Constants::DefaultClientHostPort));
			CentralLib::Logging::LogMessage<wchar_t>(L"Connected to server\n", true);

			while (true)
			{
				/* Basic chat implementation just for now to test the communications capabilities */

				std::string message;

				std::getline(std::cin, message);

				boost::asio::write((*connectionSocket), boost::asio::buffer(message));
				boost::asio::write((*connectionSocket), boost::asio::buffer(Definition::Delimiter));
			}
		}
	}
}

#endif