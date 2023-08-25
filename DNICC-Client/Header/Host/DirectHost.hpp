#pragma once
#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <QThread>

#include <NosLib/DynamicArray.hpp>
#include <NosLib/String.hpp>

#include <Central/Misc.hpp>
#include <Central/Communication.hpp>
#include <NosLib/Logging.hpp>

#include "..\Communication.hpp"
#include "..\ClientManagement\ClientManager.hpp"
#include "..\GlobalRoot.hpp"
#include "..\Client\SendReceive.hpp"
#include "InternalCommunication.hpp"

namespace DirectHost
{
	inline ClientManager* ConnectedClient;

	class ClientListenThread : public QThread
	{
		Q_OBJECT
	signals:
		void ClientConnected(ClientManager* connectedClient);
		void ClientDisconnected(ClientManager* disconnectedClient);
		void ReceivedMessage(Communications::MessageObject receivedMessage);

		void Exit();

	public slots:
		void HostSendMessage(std::wstring message)
		{
			if (ConnectedClient->GetClientStatus() != ClientEntry::enClientStatus::Online) /* if anything else than online (offline or anything else I add in the future) */
			{/* just return */
				return;
			}
			Communications::MessageObject::CreateSerializeSend(ConnectedClient->GetConnectionSocket(), GlobalRoot::ThisClient, message);
		}
	protected:
		void run() override
		{
			using AliasedHostResponse = InternalCommunication::HostResponse;

			/* Create TCP Acceptor on client host port */
			boost::asio::ip::tcp::acceptor acceptor((*GlobalRoot::IOContext), boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), std::stoi(Constants::DefaultClientHostPort)));

			NosLib::Logging::CreateLog<wchar_t>(L"Direct Server Started, waiting for a user to join", NosLib::Logging::Severity::Info, true);

			boost::system::error_code error;

			/* create new connection socket */
			acceptor.listen();

			/* accept incoming connection and assigned it to the tcp_connection_handle object socket */
			(*GlobalRoot::ConnectionSocket) = acceptor.accept(error);
			NosLib::Logging::CreateLog<wchar_t>(L"User joined", NosLib::Logging::Severity::Info, true);

			/* if there is errors */
			if (error)
			{
				NosLib::Logging::CreateLog<wchar_t>(L"Error when user was direct connecting\n", NosLib::Logging::Severity::Error, false);
				return;
			}

			bool initialValidation = true;
			while (initialValidation)
			{ /* Scoped to delete usernameBuffer after use */
				/* Get Username from Client */
				boost::asio::streambuf usernameBuffer;
				size_t lenght = boost::asio::read_until(*GlobalRoot::ConnectionSocket, usernameBuffer, Definition::Delimiter);

				std::wstring clientsUsername = Central::streamBufferToWstring(&usernameBuffer, lenght);

				if (Central::Validation::ValidateUsername(clientsUsername)) /* username is valid */
				{
					/* Create ClientTracker Object and attach it to current session */
					ConnectedClient = ClientManager::RegisterClient(clientsUsername, ClientManager::enClientStatus::Online, GlobalRoot::ConnectionSocket);
					initialValidation = false;
					AliasedHostResponse::CreateSerializeSend(ConnectedClient->GetConnectionSocket(), AliasedHostResponse::InformationCodes::Accepted, L"Direct server accepted username");
					NosLib::Logging::CreateLog<wchar_t>(L"Accepted User's username", NosLib::Logging::Severity::Info, true);
				}
				else /* username isn't valid */
				{
					AliasedHostResponse::CreateSerializeSend(ConnectedClient->GetConnectionSocket(), AliasedHostResponse::InformationCodes::NotAccepted, L"Direct server didn't accept username");
					NosLib::Logging::CreateLog<wchar_t>(L"Rejected User's username", NosLib::Logging::Severity::Info, true);
				}
			}

			NosLib::Logging::CreateLog<wchar_t>(L"User fully connected", NosLib::Logging::Severity::Info, true);
			/* Code before starting the main chat loop */
			emit ClientConnected(ConnectedClient);

			while (!isInterruptionRequested()) /* chat loop */
			{
				boost::asio::streambuf messageBuffer;
				size_t lenght = boost::asio::read_until((*ConnectedClient->GetConnectionSocket()), messageBuffer, Definition::Delimiter, error);

				if (error == boost::asio::error::eof || error == boost::asio::error::connection_reset)
				{
					break;
				}
				else if (error)
				{
					NosLib::Logging::CreateLog<wchar_t>(std::format(L"throwing error: {}", NosLib::String::ToWstring(error.what())), NosLib::Logging::Severity::Fatal, false);
					throw boost::system::system_error(error); // Some other error
				}

				Communications::MessageObject messageObject(ConnectedClient, Central::streamBufferToWstring(&messageBuffer, lenght)); /* Create message object */
				emit ReceivedMessage(messageObject);
			}

			NosLib::Logging::CreateLog<wchar_t>(L"User Disconnected", NosLib::Logging::Severity::Info, true);
			/* code after, client disconnected */
			ConnectedClient->ChangeStatus(ClientManager::enClientStatus::Offline);
			emit ClientDisconnected(ConnectedClient);

			/* just exit */
			emit Exit();
		}
	};

	inline void Exit()
	{
		GlobalRoot::ConnectionSocket->cancel();
		GlobalRoot::ConnectionSocket->close();
		NosLib::Logging::CreateLog<wchar_t>(L"destroying from server", NosLib::Logging::Severity::Info, false);

		delete ConnectedClient;

		/* Connect to DCHLS to register server */
		boost::asio::connect(*GlobalRoot::ConnectionSocket, boost::asio::ip::tcp::resolver(*GlobalRoot::IOContext).resolve(Constants::DefaultHostname, Constants::DefaultPort));
		NosLib::Logging::CreateLog<wchar_t>(L"Connected to DCHLS server to remove Direct Server\n", NosLib::Logging::Severity::Info, false);

		using AliasedClientReponse = Communications::ClientResponse;

		/* Tell server which path going down */
		AliasedClientReponse::CreateSerializeSend(GlobalRoot::ConnectionSocket, AliasedClientReponse::InformationCodes::CloseServer, GlobalRoot::UI->CreateDirectLineEdit->text().toStdWString());

		/* Disconnect from DCHLS server */
		GlobalRoot::ConnectionSocket->cancel();
		GlobalRoot::ConnectionSocket->close();
		NosLib::Logging::CreateLog<wchar_t>(L"Disconnected from DCHLS\n", NosLib::Logging::Severity::Info, false);

		GlobalRoot::UI->MainStackedWidget->setCurrentIndex(0);
		GlobalRoot::UI->ChatStackedWidget->setCurrentIndex(1);
	}

	inline void StartDirectHosting()
	{
		/* Connect to DCHLS to register server */
		boost::asio::connect(*GlobalRoot::ConnectionSocket, boost::asio::ip::tcp::resolver(*GlobalRoot::IOContext).resolve(Constants::DefaultHostname, Constants::DefaultPort));
		NosLib::Logging::CreateLog<wchar_t>(L"Connected to DCHLS server to register Direct Server\n", NosLib::Logging::Severity::Info, false);

		using AliasedClientReponse = Communications::ClientResponse;

		/* TODO: ADD VALIDATION HERE */
		/* Tell server which path going down */
		AliasedClientReponse::CreateSerializeSend(GlobalRoot::ConnectionSocket, AliasedClientReponse::InformationCodes::RegisterDirectServer, GlobalRoot::UI->CreateDirectLineEdit->text().toStdWString());

		/* Disconnect from DCHLS server */
		GlobalRoot::ConnectionSocket->cancel();
		GlobalRoot::ConnectionSocket->close();
		NosLib::Logging::CreateLog<wchar_t>(L"Disconnected from DCHLS\n", NosLib::Logging::Severity::Info, false);

		ClientListenThread* listenThread = new ClientListenThread;
		NosLib::Logging::CreateLog<wchar_t>(L"Created Listen thread\n", NosLib::Logging::Severity::Info, false);
		listenThread->start();

		//Disconnect::listenThread = listenThread;

		QObject::connect(listenThread, &ClientListenThread::ClientConnected, GlobalRoot::UI->ChatFeedScroll, &ChatFeed::ClientConnected);
		QObject::connect(listenThread, &ClientListenThread::ClientDisconnected, GlobalRoot::UI->ChatFeedScroll, &ChatFeed::ClientDisconnected);
		QObject::connect(listenThread, &ClientListenThread::ReceivedMessage, GlobalRoot::UI->ChatFeedScroll, &ChatFeed::ReceiveMessage);
		QObject::connect(listenThread, &ClientListenThread::Exit, &Exit);
		QObject::connect(&SendReceive::ChatSend::GetInstance(), &SendReceive::ChatSend::HostSendMessage, listenThread, &ClientListenThread::HostSendMessage);
		/* connect "AboutToQuit" signal to thread's interrupt signal */
		QObject::connect(GlobalRoot::AppPointer, &QCoreApplication::aboutToQuit, listenThread, &QThread::requestInterruption);
	}

	inline void ValidateHostUsername()
	{
		/* if username didn't validate */
		if (!Central::Validation::ValidateUsername(GlobalRoot::UI->LoginLineEdit->text().toStdWString()))
		{
			GlobalRoot::UI->LoginErrorLabel->setText(QStringLiteral("Username invalid, must be between 1 and 30 characters"));
			return;
		}

		GlobalRoot::ThisClient = SelfClient::RegisterSelf(GlobalRoot::UI->LoginLineEdit->text().toStdWString(), ClientManager::enClientStatus::Online, SelfClient::enClientType::Host);
		GlobalRoot::UI->ChatStackedWidget->setCurrentIndex(1);
		StartDirectHosting();
	}

	inline void InitializeDirectHosting()
	{
		/* Go to login page so user creates a user object */
		GlobalRoot::UI->MainStackedWidget->setCurrentIndex(1);
		GlobalRoot::UI->ChatStackedWidget->setCurrentIndex(0);
		QObject::connect(GlobalRoot::UI->LoginButton, &QPushButton::released, &ValidateHostUsername);

	}
}