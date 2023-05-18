#ifndef _CLIENT_HOSTING_NOSNET_HPP_
#define _CLIENT_HOSTING_NOSNET_HPP_

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <NosStdLib/DynamicArray.hpp>
#include <NosStdLib/String.hpp>

#include <Central/CentralLib.hpp>
#include <Central/Logging.hpp>


namespace ClientLib
{
	namespace Hosting
	{
		void StartServer()
		{
			boost::asio::io_context io_context;
			boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), std::stoi(Constants::DefaultClientHostPort)));

			CentralLib::Logging::LogMessage<wchar_t>(L"Client Server started\n", true);
		}
	}
}
#endif