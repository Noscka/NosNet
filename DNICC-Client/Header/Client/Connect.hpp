#ifndef _CLIENT_CONNECTION_NOSNET_
#define _CLIENT_CONNECTION_NOSNET_

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

#include <Central/CentralLib.hpp>
#include <Central/Logging.hpp>

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QPushButton>
#include <QThread>
#include "ui_MainWindow.h"

#include "..\GlobalRoot.hpp"
#include "..\ClientManagement\ClientManager.hpp"
#include "..\Communication.hpp"
#include "SendReceive.hpp"

namespace ClientLib
{
	namespace Connect
	{
		inline void FinishJoiningHost()
		{
			/* Create and start the listen thread, create using "new" since it will run along side the program */
			ClientLib::Client::ChatListenThread* listenThread = new ClientLib::Client::ChatListenThread;
			QObject::connect(listenThread, &ClientLib::Client::ChatListenThread::ReceivedMessage, GlobalRoot::UI->ChatFeedScroll, &ChatFeed::ReceiveMessage);
			listenThread->start();
			CentralLib::Logging::CreateLog<wchar_t>(L"Created and started Listen Thread\n", false);

			/* connect "AboutToQuit" signal to thread's interrupt signal */
			QObject::connect(GlobalRoot::AppPointer, &QCoreApplication::aboutToQuit, listenThread, &QThread::requestInterruption);
		}

		inline void ValidateUsername()
		{
			std::string username = GlobalRoot::UI->LoginLineEdit->text().toStdString();

			if (!CentralLib::Validation::ValidateUsername(NosLib::String::ConvertString<wchar_t, char>(username)))
			{ /* if username didn't pass username requirements */
				CentralLib::Logging::CreateLog<wchar_t>(L"Username cannot be empty and cannot be longer then 30 characters\n", false);
				GlobalRoot::UI->LoginErrorLabel->setText(QString::fromStdWString(L"Username cannot be empty and cannot be longer then 30 characters"));
				return;
			}

			/* If valid, send username to server */
			CentralLib::Write(GlobalRoot::ConnectionSocket, boost::asio::buffer(username));

			/* Wait for server response on if it accepted the username */
			boost::asio::streambuf serverReponseBuffer;
			boost::asio::read_until((*GlobalRoot::ConnectionSocket), serverReponseBuffer, Definition::Delimiter);
			ClientLib::InternalCommunication::InternalHostResponse serverReponse(&serverReponseBuffer);

			if (serverReponse.GetInformationCode() == ClientLib::InternalCommunication::InternalHostResponse::InformationCodes::Accepted) /* if server accepts username too, continue as normal */
			{
				CentralLib::Logging::CreateLog<wchar_t>((serverReponse.GetAdditionalInformation() + L"\n"), false);
				GlobalRoot::UI->LoginErrorLabel->setText(QString::fromStdWString(serverReponse.GetAdditionalInformation()));
			}
			else if (serverReponse.GetInformationCode() == ClientLib::InternalCommunication::InternalHostResponse::InformationCodes::NotAccepted) /* if server doesn't accept username, return */
			{
				CentralLib::Logging::CreateLog<wchar_t>((serverReponse.GetAdditionalInformation() + L"\n"), false);
				GlobalRoot::UI->LoginErrorLabel->setText(QString::fromStdWString(serverReponse.GetAdditionalInformation()));
				return;
			}
			else /* if server sends an unexpected response, exit because client and server are out of sync */
			{
				CentralLib::Logging::CreateLog<wchar_t>(L"server sent an unexpected response\tExiting...\n", false);
				GlobalRoot::UI->LoginErrorLabel->setText(QString::fromStdWString(L"server sent an unexpected response\nExiting..."));
				Sleep(1000);
				QApplication::exit(EXIT_FAILURE);
				exit(EXIT_FAILURE);
				return;
			}

			GlobalRoot::ThisClient = ClientLib::ClientManager::RegisterSelf(NosLib::String::ToWstring(username), ClientLib::ClientManager::enClientStatus::Online);

			/* go to chat page */
			GlobalRoot::UI->ChatStackedWidget->setCurrentIndex(1);
			FinishJoiningHost();
		}

		inline void ServerConnect(CentralLib::ServerEntry* server)
		{
			/* Connect to the Client Server (DNICC not DCHLS) */
			boost::asio::connect((*GlobalRoot::ConnectionSocket), boost::asio::ip::tcp::resolver((*GlobalRoot::IOContext)).resolve(NosLib::String::ToString(server->GetIpAddressAsWString()), Constants::DefaultClientHostPort));
			CentralLib::Logging::CreateLog<wchar_t>(L"Connected to DNICC Server\n", false);

			/* Go to login page so user creates a user object */
			GlobalRoot::UI->MainStackedWidget->setCurrentIndex(1);
			GlobalRoot::UI->ChatStackedWidget->setCurrentIndex(0);
			QObject::connect(GlobalRoot::UI->LoginButton, &QPushButton::released, &ClientLib::Connect::ValidateUsername);
		}
	}
}
#endif /* _CLIENT_CONNECTION_NOSNET_ */