#ifndef _CLIENT_RUNTIME_NOSNET_HPP_
#define _CLIENT_RUNTIME_NOSNET_HPP_

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

#include <NosStdLib/Chat.hpp>

#include <Central/CentralLib.hpp>

#include "Communication.hpp"

namespace ClientLib
{
	namespace Client
	{
		namespace /* PRIVATE NAMESPACE */
		{
			/// <summary>
			/// Returns a IP Address of a server that is chosen by the user
			/// </summary>
			/// <returns>IP Address of a server</returns>
			std::string ChooseServer()
			{
				/* Aliased with using StrippedClientTracker */
				using AliasedStrippedClientTracker = CentralLib::ClientInterfacing::StrippedClientTracker;

				/* List all clients */
				AliasedStrippedClientTracker::ListClientArray();

				bool choosing = true;
				int serverIndex = -1;
				std::string connectionChoiceString;

				while (choosing)
				{
					(void)wprintf(L"Enter Server Index: "); /* TEMP NUMBER INPUT */

					std::getline(std::cin, connectionChoiceString);

					if (sscanf_s(connectionChoiceString.c_str(), "%d", &serverIndex) != 1)
					{ /* Conversion failed */
						(void)wprintf(L"Invalid argument, please input again\n");
						continue;
					}

					if (serverIndex < 0 || serverIndex > AliasedStrippedClientTracker::GetClientArray()->GetArrayIndexPointer() - 1) /* TODO: Validate range */
					{ /* out of range */
						(void)wprintf(L"input was out of range\n");
						continue;
					}

					choosing = false; /* Past all checks, is valid */
				}

				return (*AliasedStrippedClientTracker::GetClientArray())[serverIndex]->ReturnIPAddress();
			}

			void ChatListen_Thread(boost::asio::ip::tcp::socket* connectionSocket, NosStdLib::Chat::DynamicChat* mainChat)
			{
				boost::system::error_code errorCode;
				while (mainChat->GetChatLoopState() && errorCode != boost::asio::error::eof)
				{
					boost::asio::streambuf MessageBuffer;
					size_t size = boost::asio::read_until((*connectionSocket), MessageBuffer, Definition::Delimiter, errorCode);

					ClientLib::Communications::MessageObject messageObject(&MessageBuffer); /* Create message object */

					mainChat->AddMessage(std::format(L"{}) {}", messageObject.GetUserInfo()->GetUsername() ,messageObject.GetMessage()));
				}
			}

			boost::asio::ip::tcp::socket* ConnectionSocket; /* TEMP */

			void OnMessageSentEvent(const std::wstring& message)
			{
				std::string temp = NosStdLib::String::ConvertString<char, wchar_t>(message);
				CentralLib::Write(ConnectionSocket, boost::asio::buffer(temp));
			}
		}

		/// <summary>
		/// Function ran if User chose to be a client
		/// </summary>
		/// <param name="connectionSocket">- Pointer to connection socket</param>
		void StartClient(boost::asio::io_context* io_context, boost::asio::ip::tcp::socket* connectionSocket)
		{
			ConnectionSocket = connectionSocket; /* TEMP */

			/* Aliased with using StrippedClientTracker */
			using AliasedStrippedClientTracker = CentralLib::ClientInterfacing::StrippedClientTracker;
			using AliasedClientReponse = ClientLib::Communications::ClientResponse;

			/* Tell server which path going down */
			AliasedClientReponse::CreateSerializeSend(connectionSocket, AliasedClientReponse::InformationCodes::GoingClientPath, L"User is Client");

			/* Once sent username, tell server client is ready for Array */
			AliasedClientReponse::CreateSerializeSend(connectionSocket, AliasedClientReponse::InformationCodes::Ready, L"Client is ready for array");

			/* Receive array */
			boost::asio::streambuf ContentBuffer;
			boost::asio::read_until((*connectionSocket), ContentBuffer, Definition::Delimiter);
			AliasedStrippedClientTracker::DeserializeArray(&ContentBuffer);

			/* List the array */
			(void)wprintf(AliasedStrippedClientTracker::ListClientArray().c_str());

			/* Disconnect from DCHLS server */
			(*connectionSocket).cancel();

			/* TEMP: put into variable and then use */
			std::string ipAddress = ChooseServer();

			/*
			Connect to the Client Server (DNICC not DCHLS)
			*/
			boost::asio::connect((*connectionSocket), boost::asio::ip::tcp::resolver((*io_context)).resolve(ipAddress, Constants::DefaultClientHostPort));
			CentralLib::Logging::LogMessage<wchar_t>(L"Connected to server\n", true);

			ClientLib::StartUp::GatherUsername(connectionSocket);

			NosStdLib::Chat::DynamicChat mainChat;

			std::thread chatListenThread(&ChatListen_Thread, connectionSocket, &mainChat);

			mainChat.OnMessageSent.AssignEventFunction(&OnMessageSentEvent);

			NosStdLib::Console::ClearScreen();
			mainChat.StartChat();

			chatListenThread.join();
		}
	}
}
#endif