#ifndef _CLIENT_HOSTING_NOSNET_HPP_
#define _CLIENT_HOSTING_NOSNET_HPP_

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <QThread>

#include <NosLib/DynamicArray.hpp>
#include <NosLib/String.hpp>

#include <Central/CentralLib.hpp>
#include <Central/Communication.hpp>
#include <Central/Logging.hpp>
#include "..\DCHLS-Server\Header\Communication.hpp" /* TEMP */

#include "..\Communication.hpp"
#include "..\ClientManagement\ClientManager.hpp"
#include "..\GlobalRoot.hpp"

namespace ClientLib
{
	namespace Hosting
	{
		class ClientConnectionHandle : public QThread
		{
			Q_OBJECT

		signals:
			void ClientConnected(ClientLib::ClientManager* connectedClient);

		private:
			boost::asio::ip::tcp::socket ConnectionSocket;
			ClientLib::ClientManager* ClientManagerAttached;

			ClientConnectionHandle(boost::asio::io_context& io_context) : ConnectionSocket(io_context) {}

			void UserDisconnects()
			{
				ClientManagerAttached->ChangeStatus(ClientLib::ClientManager::enClientStatus::Offline);

				ClientLib::Communications::MessageObject tempMessageObject(ClientManagerAttached, L"Client has left");

				SendToAll(&tempMessageObject, false);
			}

			void SendToAll(ClientLib::Communications::MessageObject* messageObject, const bool& sendToSelfClient)
			{
				boost::asio::streambuf tempStreamBuf;
				messageObject->SerializeObject(&tempStreamBuf); /* serialize into a buffer, which will be used to send to everyone */

				for (ClientLib::ClientEntry* clientEntry_ : ClientLib::ClientEntry::ClientRegistry)
				{
					ClientLib::ClientManager* clientEntry = (ClientLib::ClientManager*)clientEntry_;

					if (clientEntry == ClientManagerAttached && !sendToSelfClient)
					{
						continue;
					}

					CentralLib::Write(clientEntry->GetConnectionSocket(), tempStreamBuf);
				}
			}

			void StartImp()
			{
				using AliasedServerReponse = ServerLib::Communications::ServerResponse;

				CentralLib::Logging::CreateLog<wchar_t>(std::format(L"Client Connected from {}\n", CentralLib::ReturnAddress(ConnectionSocket.remote_endpoint())), false);

				CentralLib::Logging::CreateLog<wchar_t>(L"Creating profile and adding to array\n", false);

				bool initialValidation = true;
				while (initialValidation)
				{ /* Scoped to delete usernameBuffer after use */
					/* Get Username from Client */
					boost::asio::streambuf usernameBuffer;
					size_t lenght = boost::asio::read_until(ConnectionSocket, usernameBuffer, Definition::Delimiter);

					std::wstring clientsUsername = CentralLib::streamBufferToWstring(&usernameBuffer, lenght);

					if (CentralLib::Validation::ValidateUsername(clientsUsername)) /* username is valid */
					{
						/* Create ClientTracker Object and attach it to current session */
						ClientManagerAttached = ClientLib::ClientManager::RegisterClient(clientsUsername, ClientLib::ClientManager::enClientStatus::Online, &ConnectionSocket);
						initialValidation = false;
						AliasedServerReponse::CreateSerializeSend(&ConnectionSocket, AliasedServerReponse::InformationCodes::Accepted, L"server accepted username");
					}
					else /* username isn't valid */
					{
						AliasedServerReponse::CreateSerializeSend(&ConnectionSocket, AliasedServerReponse::InformationCodes::NotAccepted, L"server didn't accept username");
					}
				}

				/* user connected and is validated */
				emit ClientConnected(ClientManagerAttached);

				try
				{
					while (true)
					{
						boost::system::error_code error;

						boost::asio::streambuf messageBuffer;
						size_t lenght = boost::asio::read_until(ConnectionSocket, messageBuffer, Definition::Delimiter);

						if (error == boost::asio::error::eof)
						{
							break; // Connection closed cleanly by client.
						}
						else if (error)
						{
							throw boost::system::system_error(error); // Some other error
						}

						ClientLib::Communications::MessageObject messageObject(ClientManagerAttached, CentralLib::streamBufferToWstring(&messageBuffer, lenght)); /* Create message object */

						SendToAll(&messageObject, false);
					}
				}
				catch (const std::exception& e)
				{
					CentralLib::Logging::CreateLog<wchar_t>(NosLib::String::ConvertString<wchar_t, char>(e.what()), true);
					std::wcerr << NosLib::String::ConvertString<wchar_t, char>(e.what()) << std::endl;
				}
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
				UserDisconnects();
			}
		};

		class ClientListenThread : public QThread
		{
			Q_OBJECT
		protected:
			void run() override
			{
				/* Create TCP Acceptor on client host port */
				boost::asio::ip::tcp::acceptor acceptor((*GlobalRoot::IOContext), boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), std::stoi(Constants::DefaultClientHostPort)));

				SetConsoleTitle(std::format(L"DNICC hosting Server - {}", CentralLib::ReturnAddress(acceptor.local_endpoint())).c_str());

				CentralLib::Logging::CreateLog<wchar_t>(L"Client Server started\n", true);

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
						//connect(newConnectionHandle, &ClientConnectionHandle::ClientConnected, GlobalRoot::UI->ClientListScroll, &ClientList::ClientConnected);
						connect(GlobalRoot::AppPointer, &QCoreApplication::aboutToQuit, newConnectionHandle, &QThread::requestInterruption);
					}
				}
			}
		};

		inline void StartHosting()
		{
			using AliasedClientReponse = ClientLib::Communications::ClientResponse;

			/* Tell server which path going down */
			//AliasedClientReponse::CreateSerializeSend(GlobalRoot::ConnectionSocket, AliasedClientReponse::InformationCodes::GoingHostingPath, L"User is Hosting");

			/* Disconnect from DCHLS server */
			GlobalRoot::ConnectionSocket->cancel();

			ClientListenThread* listenThread = new ClientListenThread;
			listenThread->start();
			CentralLib::Logging::CreateLog<wchar_t>(L"Created and started Client Listen Thread\n", false);
			//GlobalRoot::UI->stackedWidget->setCurrentIndex(4);

			/* connect "AboutToQuit" signal to thread's interrupt signal */
			QObject::connect(GlobalRoot::AppPointer, &QCoreApplication::aboutToQuit, listenThread, &QThread::requestInterruption);
		}
	}
}
#endif