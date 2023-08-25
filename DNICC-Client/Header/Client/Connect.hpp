#pragma once
#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

#include <Central/Misc.hpp>
#include <NosLib/Logging.hpp>

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
#include "..\Disconnect.hpp"

namespace Connect
{
	inline void FinishJoiningHost()
	{
		/* Create and start the listen thread, create using "new" since it will run along side the program */
		SendReceive::ChatListenThread* listenThread = new SendReceive::ChatListenThread;
		QObject::connect(listenThread, &SendReceive::ChatListenThread::ReceivedMessage, GlobalRoot::UI->ChatFeedScroll, &ChatFeed::ReceiveMessage);
		listenThread->start();
		NosLib::Logging::CreateLog<wchar_t>(L"Created and started Listen Thread\n", NosLib::Logging::Severity::Info, false);

		/* connect "AboutToQuit" signal to thread's interrupt signal */
		QObject::connect(GlobalRoot::AppPointer, &QCoreApplication::aboutToQuit, listenThread, &QThread::requestInterruption);

		Disconnect::listenThread = listenThread;
	}

	inline void ValidateUsername()
	{
		std::string username = GlobalRoot::UI->LoginLineEdit->text().toStdString();

		if (!Central::Validation::ValidateUsername(NosLib::String::ConvertString<wchar_t, char>(username)))
		{ /* if username didn't pass username requirements */
			NosLib::Logging::CreateLog<wchar_t>(L"Username cannot be empty and cannot be longer then 30 characters\n", NosLib::Logging::Severity::Warning, false);
			GlobalRoot::UI->LoginErrorLabel->setText(QString::fromStdWString(L"Username cannot be empty and cannot be longer then 30 characters"));
			return;
		}

		/* If valid, send username to server */
		Central::Write(GlobalRoot::ConnectionSocket, boost::asio::buffer(username));

		/* Wait for server response on if it accepted the username */
		boost::asio::streambuf serverReponseBuffer;
		boost::asio::read_until((*GlobalRoot::ConnectionSocket), serverReponseBuffer, Definition::Delimiter);
		InternalCommunication::InternalHostResponse serverReponse(&serverReponseBuffer);

		if (serverReponse.GetInformationCode() == InternalCommunication::InternalHostResponse::InformationCodes::Accepted) /* if server accepts username too, continue as normal */
		{
			NosLib::Logging::CreateLog<wchar_t>((serverReponse.GetAdditionalInformation() + L"\n"), NosLib::Logging::Severity::Info, false);
			GlobalRoot::UI->LoginErrorLabel->setText(QString::fromStdWString(serverReponse.GetAdditionalInformation()));
		}
		else if (serverReponse.GetInformationCode() == InternalCommunication::InternalHostResponse::InformationCodes::NotAccepted) /* if server doesn't accept username, return */
		{
			NosLib::Logging::CreateLog<wchar_t>((serverReponse.GetAdditionalInformation() + L"\n"), NosLib::Logging::Severity::Info, false);
			GlobalRoot::UI->LoginErrorLabel->setText(QString::fromStdWString(serverReponse.GetAdditionalInformation()));
			return;
		}
		else /* if server sends an unexpected response, exit because client and server are out of sync */
		{
			NosLib::Logging::CreateLog<wchar_t>(L"server sent an unexpected response\tExiting...\n", NosLib::Logging::Severity::Fatal, false);
			GlobalRoot::UI->LoginErrorLabel->setText(QString::fromStdWString(L"server sent an unexpected response\nExiting..."));
			Sleep(1000);
			QApplication::exit(EXIT_FAILURE);
			exit(EXIT_FAILURE);
			return;
		}

		GlobalRoot::ThisClient = SelfClient::RegisterSelf(NosLib::String::ToWstring(username), ClientManager::enClientStatus::Online, SelfClient::enClientType::Client);

		/* go to chat page */
		GlobalRoot::UI->MainStackedWidget->setCurrentIndex(1);
		GlobalRoot::UI->ChatStackedWidget->setCurrentIndex(1);
		FinishJoiningHost();
	}

	inline void ServerConnect(Central::ServerEntry* server)
	{
		/* Connect to the Client Server (DNICC not DCHLS) */
		boost::asio::connect((*GlobalRoot::ConnectionSocket), boost::asio::ip::tcp::resolver((*GlobalRoot::IOContext)).resolve(NosLib::String::ToString(server->GetIpAddressAsWString()), Constants::DefaultClientHostPort));
		NosLib::Logging::CreateLog<wchar_t>(L"Connected to DNICC Server\n", NosLib::Logging::Severity::Info, false);

		/* Go to login page so user creates a user object */
		GlobalRoot::UI->MainStackedWidget->setCurrentIndex(1);
		GlobalRoot::UI->ChatStackedWidget->setCurrentIndex(0);
		QObject::connect(GlobalRoot::UI->LoginButton, &QPushButton::released, &Connect::ValidateUsername);
	}
}