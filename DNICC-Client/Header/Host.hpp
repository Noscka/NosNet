#ifndef _CLIENT_HOSTING_NOSNET_HPP_
#define _CLIENT_HOSTING_NOSNET_HPP_

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <NosLib/DynamicArray.hpp>
#include <NosLib/String.hpp>

#include <Central/CentralLib.hpp>
#include <Central/Communication.hpp>
#include <Central/Logging.hpp>
#include "..\DCHLS-Server-console\Header\ServerLib.hpp" /* TEMP */

#include "ClientLib.hpp"
#include "Communication.hpp"

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
				ClientTrackerAttached->ChangeStatus(ClientLib::ClientInterfacing::StrippedClientTracker::UserStatus::Offline);

				ClientLib::Communications::MessageObject tempMessageObject(ClientTrackerAttached, L"Client has left");

				SendToAll(&tempMessageObject, false);
			}

			void SendToAll(ClientLib::Communications::MessageObject* messageObject, const bool& sendToSelfClient)
			{
				boost::asio::streambuf tempStreamBuf;
				messageObject->SerializeObject(&tempStreamBuf); /* serialize into a buffer, which will be used to send to everyone */

				for (CentralLib::ClientInterfacing::StrippedClientTracker* singleClient : *(ClientTrackerAttached->GetClientArray()))
				{
					if (singleClient == ClientTrackerAttached && !sendToSelfClient)
					{
						continue;
					}

					CentralLib::Write(((CentralLib::ClientManagement::ClientTracker*)singleClient)->GetConnectionSocket(), tempStreamBuf);
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
						ClientTrackerAttached = CentralLib::ClientManagement::ClientTracker::RegisterClient(clientsUsername, ClientLib::ClientInterfacing::StrippedClientTracker::UserStatus::Client, &ConnectionSocket);
						initialValidation = false;
						AliasedServerReponse::CreateSerializeSend(&ConnectionSocket, AliasedServerReponse::InformationCodes::Accepted, L"server accepted username");
					}
					else /* username isn't valid */
					{
						AliasedServerReponse::CreateSerializeSend(&ConnectionSocket, AliasedServerReponse::InformationCodes::NotAccepted, L"server didn't accept username");
					}
				}

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

						(void)wprintf(std::format(L"{}) {}\n", ClientTrackerAttached->GetUsername(), CentralLib::streamBufferToWstring(&messageBuffer, lenght)).c_str());

						ClientLib::Communications::MessageObject messageObject(ClientTrackerAttached, CentralLib::streamBufferToWstring(&messageBuffer, lenght)); /* Create message object */

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

		inline void StartHosting(boost::asio::io_context* io_context, boost::asio::ip::tcp::socket* connectionSocket)
		{
			using AliasedClientReponse = ClientLib::Communications::ClientResponse;

			/* Tell server which path going down */
			AliasedClientReponse::CreateSerializeSend(connectionSocket, AliasedClientReponse::InformationCodes::GoingHostingPath, L"User is Hosting");

			try
			{
				/* Disconnect from DCHLS server */
				connectionSocket->cancel();

				/* Create TCP Acceptor on client host port */
				boost::asio::ip::tcp::acceptor acceptor((*io_context), boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), std::stoi(Constants::DefaultClientHostPort)));

				SetConsoleTitle(std::format(L"DNICC Client Server - {}", CentralLib::ReturnAddress(acceptor.local_endpoint())).c_str());

				CentralLib::Logging::CreateLog<wchar_t>(L"Client Server started\n", true);

				while (true)
				{
					/* tcp_connection_handle object which allows for managed of multiple users */
					tcp_connection_handle* newConSim = tcp_connection_handle::create((*io_context));

					boost::system::error_code error;

					/* accept incoming connection and assigned it to the tcp_connection_handle object socket */
					acceptor.accept(newConSim->GetSocket(), error);

					/* if no errors, create thread for the new connection */
					if (!error) { boost::thread* ClientThread = new boost::thread(boost::bind(&tcp_connection_handle::StartPublic, newConSim)); }
				}
			}
			catch (const std::exception& e)
			{
				std::wcerr << NosLib::String::ConvertString<wchar_t, char>(e.what()) << std::endl;
			}
		}
	}
}
#endif