#pragma once

#include <QThread>
#include <format>
#include "GlobalRoot.hpp"

namespace Disconnect
{
	inline QThread* listenThread;

	inline void DisconnectFromServer()
	{
		NosLib::Logging::CreateLog<wchar_t>(L"Disconnecting from server", NosLib::Logging::Severity::Info, false);

		listenThread->exit();
		listenThread->requestInterruption();
		NosLib::Logging::CreateLog<wchar_t>(L"Interrupted and set listen thread up for deletion", NosLib::Logging::Severity::Info, false);
		
		//GlobalRoot::ConnectionSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
		GlobalRoot::ConnectionSocket->cancel();
		GlobalRoot::ConnectionSocket->close();
		NosLib::Logging::CreateLog<wchar_t>(L"Disconnected from server", NosLib::Logging::Severity::Info, false);
		listenThread->wait();
		//delete GlobalRoot::ConnectionSocket;
		delete GlobalRoot::ThisClient;
	}
}