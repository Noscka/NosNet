#pragma once

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>

#include <QtWidgets/QApplication>

namespace GlobalRoot
{
	inline QApplication* AppPointer = nullptr;
}