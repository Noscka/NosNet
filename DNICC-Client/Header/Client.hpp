#ifndef _CLIENT_RUNTIME_NOSNET_HPP_
#define _CLIENT_RUNTIME_NOSNET_HPP_

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

#include <NosLib/Chat.hpp>

#include <Central/CentralLib.hpp>

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QPushButton>
#include "ui_MainWindow.h"

#include "Communication.hpp"

namespace ClientLib
{
	namespace Client /* forward deceleration */
	{
		void InitiateJoiningHost(CentralLib::ClientInterfacing::StrippedClientTracker* hostObject);
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
		namespace /* PRIVATE NAMESPACE */
		{
			Ui::MainWindowClass* UI;
			boost::asio::ip::tcp::socket* ConnectionSocket;
			boost::asio::io_context* IOContext;


			void ChatListen_Thread(NosLib::Chat::DynamicChat* mainChat)
			{
				boost::system::error_code errorCode;
				while (mainChat->GetChatLoopState() && errorCode != boost::asio::error::eof)
				{
					boost::asio::streambuf MessageBuffer;
					size_t size = boost::asio::read_until((*ConnectionSocket), MessageBuffer, Definition::Delimiter, errorCode);

					ClientLib::Communications::MessageObject messageObject(&MessageBuffer); /* Create message object */

					mainChat->AddMessage(std::format(L"{}) {}", messageObject.GetUserInfo()->GetUsername() ,messageObject.GetMessage()));
				}
			}


			void OnMessageSentEvent(const std::wstring& message)
			{
				std::string temp = NosLib::String::ConvertString<char, wchar_t>(message);
				CentralLib::Write(ConnectionSocket, boost::asio::buffer(temp));
			}
		}

		/// <summary>
		/// Function ran if User chose to be a client
		/// </summary>
		/// <param name="connectionSocket">- Pointer to connection socket</param>
		inline void StartClient(Ui::MainWindowClass* ui, boost::asio::io_context* ioContext, boost::asio::ip::tcp::socket* connectionSocket)
		{
			UI = ui;
			ConnectionSocket = connectionSocket;
			IOContext = ioContext;

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
			(*ConnectionSocket).cancel();

			/* Connect to the Client Server (DNICC not DCHLS) */
			boost::asio::connect((*ConnectionSocket), boost::asio::ip::tcp::resolver((*IOContext)).resolve(hostObject->ReturnIPAddress(), Constants::DefaultClientHostPort));
			CentralLib::Logging::CreateLog<wchar_t>(L"Connected to DNICC Server\n", false);

			/* Go to Chat page */
			UI->stackedWidget->setCurrentIndex(2);
		}

		inline void FinishJoiningHost(); /* forward deceleration */

		inline void ValidateUsername()
		{
			std::string username = UI->LoginTextbox->text().toStdString();

			if (!CentralLib::Validation::ValidateUsername(NosLib::String::ConvertString<wchar_t, char>(username)))
			{ /* if username didn't pass username requirements */
				CentralLib::Logging::CreateLog<wchar_t>(L"Username cannot be empty and cannot be longer then 30 characters\n", false);
				UI->LoginErrorLabel->setText(QString::fromStdWString(L"Username cannot be empty and cannot be longer then 30 characters"));
				return;
			}

			/* If valid, send username to server */
			CentralLib::Write(ConnectionSocket, boost::asio::buffer(username));

			/* Wait for server response on if it accepted the username */
			boost::asio::streambuf serverReponseBuffer;
			boost::asio::read_until((*ConnectionSocket), serverReponseBuffer, Definition::Delimiter);
			CentralLib::Communications::CentralizedServerResponse serverReponse(&serverReponseBuffer);

			if (serverReponse.GetInformationCode() == CentralLib::Communications::CentralizedServerResponse::InformationCodes::Accepted) /* if server accepts username too, continue as normal */
			{
				CentralLib::Logging::CreateLog<wchar_t>((serverReponse.GetAdditionalInformation() + L"\n"), false);
				UI->LoginErrorLabel->setText(QString::fromStdWString(serverReponse.GetAdditionalInformation()));
			}
			else if (serverReponse.GetInformationCode() == CentralLib::Communications::CentralizedServerResponse::InformationCodes::NotAccepted) /* if server doesn't accept username, return */
			{
				CentralLib::Logging::CreateLog<wchar_t>((serverReponse.GetAdditionalInformation() + L"\n"), false);
				UI->LoginErrorLabel->setText(QString::fromStdWString(serverReponse.GetAdditionalInformation()));
				return;
			}
			else /* if server sends an unexpected response, exit because client and server are out of sync */
			{
				CentralLib::Logging::CreateLog<wchar_t>(L"server sent an unexpected response\nExiting...\n", false);
				UI->LoginErrorLabel->setText(QString::fromStdWString(L"server sent an unexpected response\nExiting..."));
				Sleep(1000);
				QApplication::quit();
				exit(EXIT_FAILURE);
				return;
			}

			/* go to chat page */
			UI->stackedWidget->setCurrentIndex(3);
			FinishJoiningHost();
		}

		inline void FinishJoiningHost()
		{
			//std::thread chatListenThread(&ChatListen_Thread);
			//
			//mainChat.OnMessageSent.AssignEventFunction(&OnMessageSentEvent);
			//
			//NosLib::Console::ClearScreen();
			//mainChat.StartChat();
			//
			//chatListenThread.join();
		}
	}
}
#endif