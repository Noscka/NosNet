#ifndef _CLIENT_RUNTIME_NOSNET_HPP_
#define _CLIENT_RUNTIME_NOSNET_HPP_

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

namespace ClientLib
{
	namespace Runtime
	{
		/// <summary>
		/// Function ran if User chose to be a client
		/// </summary>
		/// <param name="connectionSocket">- Pointer to connection socket</param>
		void NormalClient(boost::asio::ip::tcp::socket* connectionSocket) /* Rename to something more describtive */
		{
			ClientLib::StartUp::GatherUsername(connectionSocket);

			boost::asio::streambuf ContentBuffer;
			boost::asio::read_until((*connectionSocket), ContentBuffer, Definition::Delimiter);

			CentralLib::ClientInterfacing::StrippedClientTracker::DeserializeArray(&ContentBuffer);

			wprintf(CentralLib::ClientInterfacing::StrippedClientTracker::ListClientArray().c_str());


		}
	}
}

#endif