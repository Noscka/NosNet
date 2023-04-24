#ifndef _CLIENT_LIBRARY_NOSNET_HPP_
#define _CLIENT_LIBRARY_NOSNET_HPP_

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <string>

namespace ClientLib
{
    namespace Constants
    {
        /* Default Connection info */
        inline const std::string DefaultPort = "58233";
        inline const std::string DefaultHostname = "localhost";

        /* Connection info for update service */
        inline const std::string UpdateServiceHostName = DefaultHostname;
        inline const std::string UpdateServicePort = DefaultPort;
    }
}
#endif