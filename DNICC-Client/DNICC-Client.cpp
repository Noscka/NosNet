#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>

#include <NosStdLib/Console.hpp>
#include <NosStdLib/String.hpp>

#include <Central/CentralLib.hpp>
#include <Client/MainClient/ClientLib.hpp>

#include <iostream>
#include <conio.h>

int main()
{
    NosStdLib::Console::InitializeModifiers::EnableUnicode();
    NosStdLib::Console::InitializeModifiers::EnableANSI();
    NosStdLib::Console::InitializeModifiers::BeatifyConsole<wchar_t>(L"DNICC Client");
    NosStdLib::Console::InitializeModifiers::InitializeEventHandler();

    boost::asio::io_context io_context;

    boost::asio::ip::tcp::socket socket(io_context);

    try
    {
        wprintf(L"(DEBUGGING) Type in hostname: ");
        std::string hostName;
        std::getline(std::cin, hostName);
        if (hostName.empty())
        {
            hostName = ClientLib::Constants::DefaultHostname;
        }

        /*
        Connects to the function using `resolver` which resolves the address e.g. (Noscka.com -> 123.123.123.123)
        Host - Hostname/Ip address
        Service - Service(Hostname for ports)/Port number
        */
        boost::asio::connect(socket, boost::asio::ip::tcp::resolver(io_context).resolve(hostName, ClientLib::Constants::DefaultPort));
        wprintf(L"Connected to server\n");

		std::string username;

        { /* scoped to destroy bool */
            bool gatheringUsername = true;

            while (gatheringUsername) /* Client side Verifications */
            {
				wprintf(L"Type in a username: ");
				std::getline(std::cin, username);

                if (CentralLib::Validation::ValidateUsername(NosStdLib::String::ConvertString<wchar_t, char>(username)))
                {
					gatheringUsername = false;

                    /* If valid, send username to server */
					boost::asio::write(socket, boost::asio::buffer(username));
					boost::asio::write(socket, boost::asio::buffer(Definition::Delimiter));

                    /* Wait for server response on if it accepted the username */
					boost::asio::streambuf serverReponseBuffer;
					boost::asio::read_until(socket, serverReponseBuffer, Definition::Delimiter);

					CentralLib::Communications::CentralizedServerResponse serverReponse;
					serverReponse.DeserializeObject(&serverReponseBuffer);

					if (serverReponse.GetInformationCode() == CentralLib::Communications::CentralizedServerResponse::Accepted) /* if server accepts username too, continue as normal */
					{
						wprintf((serverReponse.GetAdditionalInformation() + L"\n").c_str());
					}
					else if (serverReponse.GetInformationCode() == CentralLib::Communications::CentralizedServerResponse::NotAccepted) /* if server doesn't accept username, don't exit loop */
					{
						wprintf((serverReponse.GetAdditionalInformation() + L"\n").c_str());
						gatheringUsername = true;
					}
					else /* if server sends an unexpected response, exit because client and server are out of sync */
					{
						wprintf(L"server sent an unexpected response\nExiting...\n");
						Sleep(1000);
						exit(-1);
					}
				}
                else
                {
					wprintf(L"Username cannot be empty and cannot be longer then 30 characters\n");
				}
            }
        }

		boost::asio::streambuf ContentBuffer;
		boost::asio::read_until(socket, ContentBuffer, Definition::Delimiter);

		CentralLib::ClientInterfacing::StrippedClientTracker::DeserializeArray(&ContentBuffer);

        wprintf(CentralLib::ClientInterfacing::StrippedClientTracker::ListClientArray().c_str());
    }
    catch (const std::exception& e)
    {
        std::wcerr << NosStdLib::String::ConvertString<wchar_t, char>(e.what()) << std::endl;
    }

    wprintf(L"Press any button to continue"); _getch();
    return 0;
}