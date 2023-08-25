#pragma once
#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

#include <Central/Misc.hpp>
#include <Central/Communication.hpp>
#include <NosLib/Logging.hpp>

#include "ServerManager.hpp"

namespace Processing
{
	inline void RequestedServerArray(boost::asio::ip::tcp::socket* connectionSocket)
	{
		boost::asio::streambuf streamBuffer;
		Central::ServerEntry::SerializeArray(&streamBuffer);

		Central::Write(connectionSocket, streamBuffer);

		NosLib::Logging::CreateLog<wchar_t>(L"Sent Server Array\n", NosLib::Logging::Severity::Info, false);
	}

	inline void RegisterDirectServer(boost::asio::ip::tcp::socket* connectionSocket, ServerManager** serverManager, const std::wstring& serverName)
	{
		(*serverManager) = ServerManager::RegisterServer(serverName, ServerManager::enServerType::Direct, ServerManager::enServerStatus::Online, connectionSocket);
	}

	inline void RemoveServer(boost::asio::ip::tcp::socket* connectionSocket, ServerManager** serverManager)
	{
		(*serverManager) = ServerManager::RemoveServer(connectionSocket);
	}
}