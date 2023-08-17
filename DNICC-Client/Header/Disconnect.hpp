#pragma once

#include <QThread>
#include "GlobalRoot.hpp"

namespace Disconnect
{
	inline QThread* listenThread;

	inline void DisconnectFromServer()
	{
		listenThread->requestInterruption();
		listenThread->deleteLater();
		GlobalRoot::ConnectionSocket->close();
		//delete GlobalRoot::ThisClient;
	}
}