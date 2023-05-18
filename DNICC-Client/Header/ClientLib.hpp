#ifndef _CLIENT_LIBRARY_NOSNET_HPP_
#define _CLIENT_LIBRARY_NOSNET_HPP_

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <string>

namespace ClientLib
{
	namespace Mode
	{
		enum class ClientMode : uint8_t
		{
			Standard = 0,
			Hosting = 1,
		};
	}

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
        std::string GatherUsername(boost::asio::ip::tcp::socket* connectionSocket)
        {
			std::string username;
			bool gatheringUsername = true;

			while (gatheringUsername) /* Client side Verifications */
			{
				wprintf(L"Type in a username: ");
				std::getline(std::cin, username);

				if (!CentralLib::Validation::ValidateUsername(NosStdLib::String::ConvertString<wchar_t, char>(username)))
				{ /* if username didn't pass username requirements */
					CentralLib::Logging::LogMessage<wchar_t>(L"Username cannot be empty and cannot be longer then 30 characters\n", true);
					continue;
				}

				gatheringUsername = false;

				/* If valid, send username to server */
				boost::asio::write((*connectionSocket), boost::asio::buffer(username));
				boost::asio::write((*connectionSocket), boost::asio::buffer(Definition::Delimiter));

				/* Wait for server response on if it accepted the username */
				boost::asio::streambuf serverReponseBuffer;
				boost::asio::read_until((*connectionSocket), serverReponseBuffer, Definition::Delimiter);

				CentralLib::Communications::CentralizedServerResponse serverReponse(&serverReponseBuffer);

				if (serverReponse.GetInformationCode() == CentralLib::Communications::CentralizedServerResponse::InformationCodes::Accepted) /* if server accepts username too, continue as normal */
				{
					CentralLib::Logging::LogMessage<wchar_t>((serverReponse.GetAdditionalInformation() + L"\n"), true);
				}
				else if (serverReponse.GetInformationCode() == CentralLib::Communications::CentralizedServerResponse::InformationCodes::NotAccepted) /* if server doesn't accept username, don't exit loop */
				{
					CentralLib::Logging::LogMessage<wchar_t>((serverReponse.GetAdditionalInformation() + L"\n"), true);
					gatheringUsername = true;
				}
				else /* if server sends an unexpected response, exit because client and server are out of sync */
				{
					CentralLib::Logging::LogMessage<wchar_t>(L"server sent an unexpected response\nExiting...\n", true);
					Sleep(1000);
					exit(-1);
				}
			}

			return username;
        }

		enum ClientMode : uint8_t
		{
			Normal = 0,
			Hosting = 1,
		};

		ClientMode GatherClientMode()
		{
			bool gatheringModeChoice = true;
			int mode;
			while (gatheringModeChoice)
			{
				std::string modeString;
				wprintf(L"What mode would you like to run in?\n0) Normal (connect to others)\n1) Host (others connect to you)\n: "); /* Make more advanced Choosing */
				std::getline(std::cin, modeString);

				if (sscanf_s(modeString.c_str(), "%d", &mode) != 1)
				{ /* Conversion failed */
					wprintf(L"Invalid argument, please input again\n");
					continue;
				}

				if (mode > 1 || mode < 0)
				{ /* out of range */
					wprintf(L"input was out of range\n");
					continue;
				}

				gatheringModeChoice = false; /* if it passed all the checks, set gatheringModeChoice to false as to not check for mode anymore */
			}
			return (ClientMode)mode;
		}
    }
}
#endif