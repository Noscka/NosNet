#ifndef _CLIENT_LIBRARY_NOSNET_HPP_
#define _CLIENT_LIBRARY_NOSNET_HPP_

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <string>

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStackedWidget>
#include "ui_MainWindow.h"

#include "Central\CentralLib.hpp"

namespace ClientLib
{
    /// <summary>
    /// namespace which will contains items which are to do with starting up
    /// </summary>
    namespace StartUp
    {
        /// <summary>
        /// Gather the username from the user
        /// </summary>
        /// <param name="connectionSocket">- the current connection socket</param>
        /// <returns>the username that was gathered</returns>
		inline std::string GatherUsername(boost::asio::ip::tcp::socket* connectionSocket)
        {
			std::string username;
			bool gatheringUsername = true;

			while (gatheringUsername) /* Client side Verifications */
			{
				wprintf(L"Type in a username: ");
				std::getline(std::cin, username);

				if (!CentralLib::Validation::ValidateUsername(NosLib::String::ConvertString<wchar_t, char>(username)))
				{ /* if username didn't pass username requirements */
					CentralLib::Logging::CreateLog<wchar_t>(L"Username cannot be empty and cannot be longer then 30 characters\n", false);
					continue;
				}

				gatheringUsername = false;

				/* If valid, send username to server */
				CentralLib::Write(connectionSocket, boost::asio::buffer(username));

				/* Wait for server response on if it accepted the username */
				boost::asio::streambuf serverReponseBuffer;
				boost::asio::read_until((*connectionSocket), serverReponseBuffer, Definition::Delimiter);

				CentralLib::Communications::CentralizedServerResponse serverReponse(&serverReponseBuffer);

				if (serverReponse.GetInformationCode() == CentralLib::Communications::CentralizedServerResponse::InformationCodes::Accepted) /* if server accepts username too, continue as normal */
				{
					CentralLib::Logging::CreateLog<wchar_t>((serverReponse.GetAdditionalInformation() + L"\n"), false);
				}
				else if (serverReponse.GetInformationCode() == CentralLib::Communications::CentralizedServerResponse::InformationCodes::NotAccepted) /* if server doesn't accept username, don't exit loop */
				{
					CentralLib::Logging::CreateLog<wchar_t>((serverReponse.GetAdditionalInformation() + L"\n"), false);
					gatheringUsername = true;
				}
				else /* if server sends an unexpected response, exit because client and server are out of sync */
				{
					CentralLib::Logging::CreateLog<wchar_t>(L"server sent an unexpected response\nExiting...\n", false);
					Sleep(1000);
					exit(-1);
				}
			}

			return username;
        }

		enum class UserMode : uint8_t
		{
			Client = 0,
			Hosting = 1,
		};

		inline UserMode GatherClientMode(Ui::MainWindowClass* ui)
		{
			if (ui->NormalSelection->isChecked()) /* Normal client */
			{
				return UserMode::Client;
			}
			
			if (ui->HostSelection->isChecked()) /* Host */
			{
				return UserMode::Hosting;
			}
		}
    }
}
#endif