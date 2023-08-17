#pragma once
#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <QtWidgets/QApplication>

/* this root contains items which are defined externally, so they won't cause dependency loops */

namespace GlobalRoot
{
	inline QApplication* AppPointer = nullptr;
	inline boost::asio::ip::tcp::socket* ConnectionSocket = nullptr;
	inline boost::asio::io_context* IOContext = nullptr;
}