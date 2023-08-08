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
#include "..\Communication.hpp"
#include "SendReceive.hpp"

namespace ClientLib
{
	namespace Client
	{
		void InitiateJoiningHost(CentralLib::ClientInterfacing::StrippedClientTracker* hostObject); /* forward delecration */
	}

	namespace ClientInterfacing
	{
		class StrippedClientTracker : public CentralLib::ClientInterfacing::StrippedClientTracker
		{
		public:
			/// <summary>
			/// Creates clickable entries for all servers
			/// </summary>
			/// <param name="ui">- pointer to ui</param>
			static inline void GenerateServerEntries(Ui::MainWindowClass* ui)
			{
				for (int i = 0; i <= ClientArray.GetLastArrayIndex(); i++)
				{
					CentralLib::ClientInterfacing::StrippedClientTracker* currentEntry = ClientArray[i];

					QPushButton* serverEntry = new QPushButton();
					std::wstring entryName = std::format(L"IP: {}", NosLib::String::ToWstring(currentEntry->ReturnIPAddress()));
					serverEntry->setObjectName(entryName);
					serverEntry->setText(QString::fromStdWString(entryName));
					QMainWindow::connect(serverEntry, &QPushButton::released, [currentEntry]()
					{
						ClientLib::Client::InitiateJoiningHost(currentEntry);
					});

					ui->ServerSelectionVerticalLayout->addWidget(serverEntry);
				}
			}
		};
	}

	namespace Client
	{
		/// <summary>
		/// Function ran if User chose to be a client
		/// </summary>
		/// <param name="connectionSocket">- Pointer to connection socket</param>
		inline void StartClient(Ui::MainWindowClass* ui, boost::asio::io_context* ioContext, boost::asio::ip::tcp::socket* connectionSocket)
		{
			GlobalRoot::UI = ui;
			GlobalRoot::ConnectionSocket = connectionSocket;
			GlobalRoot::IOContext = ioContext;

			/* Aliased with using StrippedClientTracker */
			using AliasedStrippedClientTracker = ClientLib::ClientInterfacing::StrippedClientTracker;
			using AliasedClientReponse = ClientLib::Communications::ClientResponse;

			/* Tell server which path going down */
			AliasedClientReponse::CreateSerializeSend(connectionSocket, AliasedClientReponse::InformationCodes::GoingClientPath, L"User is Client");

			{
				/* Wait for Server to be ready */
				boost::asio::streambuf serverReponseBuffer;
				boost::asio::read_until((*connectionSocket), serverReponseBuffer, Definition::Delimiter);
				CentralLib::Communications::CentralizedServerResponse serverReponse(&serverReponseBuffer);

				if (serverReponse.GetInformationCode() != CentralLib::Communications::CentralizedServerResponse::InformationCodes::Ready)
				{
					CentralLib::Logging::CreateLog<wchar_t>(L"Server sent unexpected response messages, escaping\n", false);
					return;
				}
			}

			/* Once sent username, tell server client is ready for Array */
			AliasedClientReponse::CreateSerializeSend(connectionSocket, AliasedClientReponse::InformationCodes::Ready, L"Client is ready for array");

			{
				/* Receive array */
				boost::asio::streambuf ContentBuffer;
				boost::asio::read_until((*connectionSocket), ContentBuffer, Definition::Delimiter);
				AliasedStrippedClientTracker::DeserializeArray(&ContentBuffer);
			}
			/* Go to Server selection page */
			ui->stackedWidget->setCurrentIndex(1);

			/* List the array */
			AliasedStrippedClientTracker::GenerateServerEntries(ui);
		}

		inline void InitiateJoiningHost(CentralLib::ClientInterfacing::StrippedClientTracker* hostObject)
		{
			/* Disconnect from DCHLS server */
			(*GlobalRoot::ConnectionSocket).cancel();

			/* Connect to the Client Server (DNICC not DCHLS) */
			boost::asio::connect((*GlobalRoot::ConnectionSocket), boost::asio::ip::tcp::resolver((*GlobalRoot::IOContext)).resolve(hostObject->ReturnIPAddress(), Constants::DefaultClientHostPort));
			CentralLib::Logging::CreateLog<wchar_t>(L"Connected to DNICC Server\n", false);

			/* Go to Chat page */
			GlobalRoot::UI->stackedWidget->setCurrentIndex(2);
		}

		inline void FinishJoiningHost(); /* forward deceleration */

		inline void ValidateUsername()
		{
			std::string username = GlobalRoot::UI->LoginTextbox->text().toStdString();

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
			CentralLib::Communications::CentralizedServerResponse serverReponse(&serverReponseBuffer);

			if (serverReponse.GetInformationCode() == CentralLib::Communications::CentralizedServerResponse::InformationCodes::Accepted) /* if server accepts username too, continue as normal */
			{
				CentralLib::Logging::CreateLog<wchar_t>((serverReponse.GetAdditionalInformation() + L"\n"), false);
				GlobalRoot::UI->LoginErrorLabel->setText(QString::fromStdWString(serverReponse.GetAdditionalInformation()));
			}
			else if (serverReponse.GetInformationCode() == CentralLib::Communications::CentralizedServerResponse::InformationCodes::NotAccepted) /* if server doesn't accept username, return */
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

			/* go to chat page */
			GlobalRoot::UI->stackedWidget->setCurrentIndex(3);
			FinishJoiningHost();
		}

		inline void FinishJoiningHost()
		{
			/* Create and start the listen thread, create using "new" since it will run along side the program */
			ChatListenThread* listenThread = new ChatListenThread;
			QObject::connect(listenThread, &ChatListenThread::ReceivedMessage, GlobalRoot::UI->ChatFeedScroll, &ChatFeed::ReceiveMessage);
			listenThread->start();
			CentralLib::Logging::CreateLog<wchar_t>(L"Created and started Listen Thread\n", false);

			/* connect "AboutToQuit" signal to thread's interrupt signal */
			QObject::connect(GlobalRoot::AppPointer, &QCoreApplication::aboutToQuit, listenThread, &QThread::requestInterruption);
		}

	}
}
#endif /* _CLIENT_CONNECTION_NOSNET_ */