#pragma once

#include <QThread>
#include "GlobalRoot.hpp"

namespace Disconnect
{
	inline void DisconnectFromServer()
	{
		GlobalRoot::ConnectionSocket->cancel();
		//delete GlobalRoot::ThisClient;
	}
}