#pragma once

#include "ClientManagement/ClientManager.hpp"

/* this root contains items which are defined internality, so they can cause dependency loop */

namespace GlobalRoot
{
	inline ClientLib::ClientManager* ThisClient;
}