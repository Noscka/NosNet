#pragma once

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

#include <QtWidgets/QApplication>
#include "ui_MainWindow.h"

namespace GlobalRoot
{
	inline QApplication* AppPointer = nullptr;
	inline Ui::MainWindowClass* UI;
	inline boost::asio::ip::tcp::socket* ConnectionSocket;
	inline boost::asio::io_context* IOContext;
}