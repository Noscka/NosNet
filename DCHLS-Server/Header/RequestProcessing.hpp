#ifndef _SERVER_CONNECTIONPROCESSING_NOSNET_HPP_
#define _SERVER_CONNECTIONPROCESSING_NOSNET_HPP_

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

#include <Central/CentralLib.hpp>
#include <Central/Communication.hpp>
#include <Central/Logging.hpp>

#include "ServerManager.hpp"

namespace ServerLib
{
	namespace Processing
	{
		inline void RequestedServerArray(boost::asio::ip::tcp::socket* connectionSocket)
		{
			boost::asio::streambuf streamBuffer;
			CentralLib::ServerEntry::SerializeArray(&streamBuffer);

			CentralLib::Write(connectionSocket, streamBuffer);

			CentralLib::Logging::CreateLog<wchar_t>(L"Sent Server Array\n", false);
		}

		inline void RegisterDirectServer(boost::asio::ip::tcp::socket* connectionSocket, ServerLib::ServerManager** serverManager, const std::wstring& serverName)
		{
			(*serverManager) = ServerLib::ServerManager::RegisterServer(serverName, ServerLib::ServerManager::enServerType::Direct, ServerLib::ServerManager::enServerStatus::Online, connectionSocket);
		}
	}
}

#endif