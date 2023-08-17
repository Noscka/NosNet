#pragma once
#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

#include <Central/Misc.hpp>
#include <Central/Communication.hpp>
#include <Central/Logging.hpp>

#include "ServerManager.hpp"

namespace Processing
{
	inline void RequestedServerArray(boost::asio::ip::tcp::socket* connectionSocket)
	{
		boost::asio::streambuf streamBuffer;
		Central::ServerEntry::SerializeArray(&streamBuffer);

		Central::Write(connectionSocket, streamBuffer);

		Central::Logging::CreateLog<wchar_t>(L"Sent Server Array\n", false);
	}

	inline void RegisterDirectServer(boost::asio::ip::tcp::socket* connectionSocket, ServerManager** serverManager, const std::wstring& serverName)
	{
		(*serverManager) = ServerManager::RegisterServer(serverName, ServerManager::enServerType::Direct, ServerManager::enServerStatus::Online, connectionSocket);
	}
}