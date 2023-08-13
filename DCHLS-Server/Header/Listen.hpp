#pragma once

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <NosLib/Console.hpp>
#include <NosLib/DynamicArray.hpp>
#include <NosLib/String.hpp>

#include <QThread>

#include <Central/CentralLib.hpp>
#include <Central/Communication.hpp>
#include <Central/Logging.hpp>
#include <Central/ServerEntry.hpp>

#include "Communication.hpp"
#include "RequestProcessing.hpp"
#include "ServerManager.hpp"
#include "GlobalRoot.hpp"

namespace ServerLib
{
	namespace Listen
	{
		class ClientConnectionHandle : public QThread
		{
			Q_OBJECT

		signals:
			void AddNewServerEntry(ServerLib::ServerManager* connectedClient);
		private:
			boost::asio::ip::tcp::socket ConnectionSocket;
			ServerLib::ServerManager* ServerManagerAttached = nullptr;

			ClientConnectionHandle(boost::asio::io_context& io_context) : ConnectionSocket(io_context) {}

			~ClientConnectionHandle()
			{
			}

			void StartImp()
			{
				CentralLib::Logging::CreateLog<wchar_t>(std::format(L"Connection from {}\n", CentralLib::ReturnAddress(ConnectionSocket.remote_endpoint())), false);

				try
				{
					/* Check what path the user takes */
					boost::asio::streambuf clientReponseBuffer;
					boost::asio::read_until(ConnectionSocket, clientReponseBuffer, Definition::Delimiter);

					CentralLib::Communications::CentralizedClientResponse clientReponse(&clientReponseBuffer);

					switch (clientReponse.GetInformationCode())
					{
					case CentralLib::Communications::CentralizedClientResponse::InformationCodes::RequestServerArray:
						CentralLib::Logging::CreateLog<wchar_t>(L"DNICC requested server array\n", false);
						ServerLib::Processing::RequestedServerArray(&ConnectionSocket);
						break;

					case CentralLib::Communications::CentralizedClientResponse::InformationCodes::RegisterDirectServer:
						CentralLib::Logging::CreateLog<wchar_t>(L"DNICC is registering a direct server\n", false);
						ServerLib::Processing::RegisterDirectServer(&ConnectionSocket, &ServerManagerAttached, clientReponse.GetAdditionalInformation());
						emit AddNewServerEntry(ServerManagerAttached);
						break;
					case CentralLib::Communications::CentralizedClientResponse::InformationCodes::RegisterGroupServer:
						CentralLib::Logging::CreateLog<wchar_t>(L"DNICC is registering a group server\n", false);
						CentralLib::Logging::CreateLog<wchar_t>(L"NOT IMPLEMENTED\n", false);
						emit AddNewServerEntry(ServerManagerAttached);
						break;
					case CentralLib::Communications::CentralizedClientResponse::InformationCodes::RegisterDedicatedServer:
						CentralLib::Logging::CreateLog<wchar_t>(L"DNICC is registering a Dedicated server\n", false);
						CentralLib::Logging::CreateLog<wchar_t>(L"NOT IMPLEMENTED\n", false);
						emit AddNewServerEntry(ServerManagerAttached);
						break;
					}

				}
				catch (const std::exception& e)
				{
					CentralLib::Logging::CreateLog<char>(e.what(), false);
					std::wcerr << NosLib::String::ConvertString<wchar_t, char>(e.what()) << std::endl;
				}

				CentralLib::Logging::CreateLog<wchar_t>(std::format(L"Connection with {} Terminated\n", CentralLib::ReturnAddress(ConnectionSocket.remote_endpoint())), false);
			}
		public:
			static ClientConnectionHandle* create(boost::asio::io_context& io_context) { return new ClientConnectionHandle(io_context); }

			boost::asio::ip::tcp::socket& GetSocket()
			{
				return ConnectionSocket;
			}

			void run() override
			{
				StartImp(); /* Run function and make sure it gets deleted */
				connect(this, &QThread::finished, this, &QThread::deleteLater); /* commit suicide if the connection ends as the object won't get used/deleted otherwise */
			}
		};

		class ClientListenThread : public QThread
		{
			Q_OBJECT
		protected:
			void run() override
			{
				/* Create TCP Acceptor on client host port */
				boost::asio::ip::tcp::acceptor acceptor((*GlobalRoot::IOContext), boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), std::stoi(Constants::DefaultPort)));

				SetConsoleTitle(std::format(L"DCHLS hosting Server - {}", CentralLib::ReturnAddress(acceptor.local_endpoint())).c_str());

				CentralLib::Logging::CreateLog<wchar_t>(L"Server started\n", false);

				while (true)
				{
					/* tcp_connection_handle object which allows for managed of multiple users */
					ClientConnectionHandle* newConnectionHandle = ClientConnectionHandle::create((*GlobalRoot::IOContext));

					boost::system::error_code error;

					/* accept incoming connection and assigned it to the tcp_connection_handle object socket */
					acceptor.accept(newConnectionHandle->GetSocket(), error);

					/* if no errors, create thread for the new connection */
					if (!error)
					{
						newConnectionHandle->start();
						connect(newConnectionHandle, &ClientConnectionHandle::AddNewServerEntry, GlobalRoot::UI->ClientListScroll, &ClientList::AddNewServerEntry);
						connect(GlobalRoot::AppPointer, &QCoreApplication::aboutToQuit, newConnectionHandle, &QThread::requestInterruption);
					}
				}
			}
		};

		inline void StartDCHLS()
		{
			ClientListenThread* listenThread = new ClientListenThread;
			CentralLib::Logging::CreateLog<wchar_t>(L"Created and started Client Listen Thread\n", false);
			listenThread->start();

			/* connect "AboutToQuit" signal to thread's interrupt signal */
			QObject::connect(GlobalRoot::AppPointer, &QCoreApplication::aboutToQuit, listenThread, &QThread::requestInterruption);
		}
	}
}