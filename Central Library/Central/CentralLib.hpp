#ifndef _CENTRAL_LIBRARY_NOSNET_HPP_
#define _CENTRAL_LIBRARY_NOSNET_HPP_

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>

#include <NosLib/String.hpp>
#include <NosLib/Cast.hpp>

#include <string.h>
#include <format>
#include <iostream>
#include <fstream>

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

namespace CentralLib
{
	inline std::wstring ReturnAddress(const boost::asio::ip::tcp::endpoint& Endpoint)
	{
		return std::format(L"{}:{}", NosLib::String::ConvertString<wchar_t, char>(Endpoint.address().to_v4().to_string()), NosLib::String::ConvertString<wchar_t, char>(std::to_string(Endpoint.port())));
	}

    /// <summary>
    /// convert stream buffer to wide string and removing delimiter
    /// </summary>
    /// <param name="streamBuffer"> - stream buffer pointer needed </param>
    /// <param name="bytes_received"> - amount of bytes received</param>
    /// <returns>wide string</returns>
    inline std::wstring streamBufferToWstring(boost::asio::streambuf* streamBuffer, const size_t& bytes_received)
    {
        return std::wstring{boost::asio::buffers_begin(streamBuffer->data()), boost::asio::buffers_begin(streamBuffer->data()) + bytes_received - Definition::Delimiter.size()};
    }

    inline size_t Write(boost::asio::ip::tcp::socket* connectedSocket, boost::asio::mutable_buffers_1 b)
    {
		size_t firstWrite = boost::asio::write((*connectedSocket), b);
		size_t secondWrite = boost::asio::write((*connectedSocket), boost::asio::buffer(Definition::Delimiter));

		return firstWrite + secondWrite;
    }

    template<typename Allocator>
	inline size_t Write(boost::asio::ip::tcp::socket* connectedSocket, boost::asio::basic_streambuf<Allocator>& b)
    {
		size_t firstWrite = boost::asio::write((*connectedSocket), boost::asio::basic_streambuf_ref<Allocator>(b));
		size_t secondWrite = boost::asio::write((*connectedSocket), boost::asio::buffer(Definition::Delimiter));

        return firstWrite + secondWrite;
    }

    namespace Validation
    {
        /// <summary>
        /// Validates usernames (central so changes are global)
        /// </summary>
        /// <param name="username">- username to validate</param>
        /// <returns>true if valid, False if invalid</returns>
        inline bool ValidateUsername(const std::wstring& username)
        {
            /*
			Current requirements:
				- Not empty
				- Not longer then 30 characters
			*/
            if (username.empty() || username.length() >= 30)
            {
                return false;
            }

            return true;
        }

		/// <summary>
		/// Validates server name (central so changes are global)
		/// </summary>
		/// <param name="username">- username to validate</param>
		/// <returns>true if valid, False if invalid</returns>
        inline bool ValidateServerName(const std::wstring& serverName)
        {
            /* just use username validation for now */
            return ValidateUsername(serverName);
        }
    }
}
#endif