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
#include <Central/Logging.hpp>

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
		void ReceivedMessage(Communications::MessageObject receivedMessage);

	public slots:
		void HostSendMessage(std::wstring message)
		{
			Communications::MessageObject::CreateSerializeSend(ConnectedClient->GetConnectionSocket(), GlobalRoot::ThisClient, message);
		}
	protected:
		void run() override
		{
			using AliasedHostResponse = InternalCommunication::HostResponse;

			/* Create TCP Acceptor on client host port */
			boost::asio::ip::tcp::acceptor acceptor((*GlobalRoot::IOContext), boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), std::stoi(Constants::DefaultClientHostPort)));

			Central::Logging::CreateLog<wchar_t>(L"Direct Server Started, waiting for a user to join\n", true);

			boost::system::error_code error;

			/* create new connection socket */
			acceptor.listen();

			/* accept incoming connection and assigned it to the tcp_connection_handle object socket */
			(*GlobalRoot::ConnectionSocket) = acceptor.accept(error);


			/* if there is errors */
			if (error)
			{
				Central::Logging::CreateLog<wchar_t>(L"Error when user was direct connecting\n", false);
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
				}
				else /* username isn't valid */
				{
					AliasedHostResponse::CreateSerializeSend(ConnectedClient->GetConnectionSocket(), AliasedHostResponse::InformationCodes::NotAccepted, L"Direct server didn't accept username");
				}
			}

			emit ClientConnected(ConnectedClient);

			while (!isInterruptionRequested() && error != boost::asio::error::eof)
			{
				boost::asio::streambuf messageBuffer;
				size_t lenght = boost::asio::read_until((*ConnectedClient->GetConnectionSocket()), messageBuffer, Definition::Delimiter);

				if (error)
				{
					throw boost::system::system_error(error); // Some other error
				}

				Communications::MessageObject messageObject(ConnectedClient, Central::streamBufferToWstring(&messageBuffer, lenght)); /* Create message object */
				emit ReceivedMessage(messageObject);
			}
		}
	};

	inline void StartDirectHosting()
	{
		/* Connect to DCHLS to register server */
		boost::asio::connect(*GlobalRoot::ConnectionSocket, boost::asio::ip::tcp::resolver(*GlobalRoot::IOContext).resolve(Constants::DefaultHostname, Constants::DefaultPort));
		Central::Logging::CreateLog<wchar_t>(L"Connected to DCHLS server to register Direct Server\n", false);

		using AliasedClientReponse = Communications::ClientResponse;

		/* TODO: ADD VALIDATION HERE */
		/* Tell server which path going down */
		AliasedClientReponse::CreateSerializeSend(GlobalRoot::ConnectionSocket, AliasedClientReponse::InformationCodes::RegisterDirectServer, GlobalRoot::UI->CreateDirectLineEdit->text().toStdWString());

		/* Disconnect from DCHLS server */
		GlobalRoot::ConnectionSocket->cancel();
		Central::Logging::CreateLog<wchar_t>(L"Disconnected from DCHLS\n", false);

		ClientListenThread* listenThread = new ClientListenThread;
		Central::Logging::CreateLog<wchar_t>(L"Created Listen thread\n", false);
		listenThread->start();

		QObject::connect(listenThread, &ClientListenThread::ClientConnected, GlobalRoot::UI->ChatFeedScroll, &ChatFeed::ClientConnected);
		QObject::connect(listenThread, &ClientListenThread::ReceivedMessage, GlobalRoot::UI->ChatFeedScroll, &ChatFeed::ReceiveMessage);
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