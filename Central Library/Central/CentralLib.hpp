#ifndef _CENTRAL_LIBRARY_NOSNET_HPP_
#define _CENTRAL_LIBRARY_NOSNET_HPP_

#include <boost/asio.hpp>

#include <NosStdLib/String.hpp>

#include <string.h>
#include <format>

namespace CentralLib
{
    namespace Definition
    {
        const std::string Delimiter = "\n\r\n\r\n\013\x4\n";
    }

    std::wstring ReturnAddress(boost::asio::ip::tcp::endpoint Endpoint)
    {
        return std::format(L"{}:{}", NosStdLib::String::ConvertString<wchar_t, char>(Endpoint.address().to_v4().to_string()), NosStdLib::String::ConvertString<wchar_t, char>(std::to_string(Endpoint.port())));
    }
}
#endif