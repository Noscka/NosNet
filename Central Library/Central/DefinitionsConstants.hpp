#pragma once
#include <string>

namespace Definition
{
	const std::string Delimiter = "\n\r\n\r\n\013\x4\n";
	typedef unsigned char byte;
}

namespace Constants
{
	/* Default Connection info */
	inline const std::string DefaultPort = "58233";
	inline const std::string DefaultHostname = "192.168.0.15"; /* my private ip */

	/* Connection info for update service */
	inline const std::string UpdateServiceHostName = DefaultHostname;
	inline const std::string UpdateServicePort = DefaultPort;

	/* Default Client host info */
	inline const std::string DefaultClientHostPort = "58234";
}