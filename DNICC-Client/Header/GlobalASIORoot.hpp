#pragma once

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

namespace GlobalRoot
{
	inline boost::asio::ip::tcp::socket* ConnectionSocket;
	inline boost::asio::io_context* IOContext;
}