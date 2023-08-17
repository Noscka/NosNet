#pragma once
#include "..\ClientManagement/ClientManager.hpp"

/* this root contains items which are defined internally, so they can cause dependency loop */

namespace GlobalRoot
{
	inline SelfClient* ThisClient;
}