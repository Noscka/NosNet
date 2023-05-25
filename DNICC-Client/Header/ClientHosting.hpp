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
		class tcp_connection_handle
		{
		private:
			boost::asio::ip::tcp::socket ConnectionSocket;
			CentralLib::ClientManagement::ClientTracker* ClientTrackerAttached;

			tcp_connection_handle(boost::asio::io_context& io_context) : ConnectionSocket(io_context) {}

			~tcp_connection_handle()
			{
				ClientTrackerAttached->ChangeStatus(CentralLib::ClientInterfacing::StrippedClientTracker::ClientStatus::Offline);

				//delete ClientTrackerAttached; /* COMMENTED OUT FOR DEBUGGING */
			}

			void StartImp()
			{
				CentralLib::Logging::LogMessage<wchar_t>(std::format(L"Client Connected from {}\n", CentralLib::ReturnAddress(ConnectionSocket.remote_endpoint())), true);

				CentralLib::Logging::LogMessage<wchar_t>(L"Creating profile and adding to array\n", true);
			}
		public:
			static tcp_connection_handle* create(boost::asio::io_context& io_context) { return new tcp_connection_handle(io_context); }

			boost::asio::ip::tcp::socket& GetSocket()
			{
				return ConnectionSocket;
			}

			void StartPublic()
			{
				StartImp(); /* Run function and make sure it gets deleted */
				delete this; /* commit suicide if the connection ends as the object won't get used/deleted otherwise */
			}
		};

		void StartServer()
		{
			try
			{
				boost::asio::io_context io_context;
				boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), std::stoi(Constants::DefaultClientHostPort)));

				CentralLib::Logging::LogMessage<wchar_t>(L"Client Server started\n", true);

				while (true)
				{
					/* tcp_connection object which allows for managed of multiple users */
					tcp_connection_handle* newConSim = tcp_connection_handle::create(io_context);

					boost::system::error_code error;

					/* accept incoming connection and assigned it to the tcp_connection object socket */
					acceptor.accept(newConSim->GetSocket(), error);

					/* if no errors, create thread for the new connection */
					if (!error) { boost::thread* ClientThread = new boost::thread(boost::bind(&tcp_connection_handle::StartPublic, newConSim)); }
				}
			}
			catch (const std::exception& e)
			{
				std::wcerr << NosStdLib::String::ConvertString<wchar_t, char>(e.what()) << std::endl;
			}
		}
	}
}
#endif