#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>

#include <NosLib/Console.hpp>
#include <NosLib/String.hpp>

#include <Central/CentralLib.hpp>
#include <Central/Logging.hpp>
#include "Header/ClientLib.hpp"
#include "Header/Client.hpp"
#include "Header/Host.hpp"

#include <iostream>
#include <conio.h>

int main()
{
    NosLib::Console::InitializeModifiers::EnableUnicode();
    NosLib::Console::InitializeModifiers::EnableANSI();
    NosLib::Console::InitializeModifiers::BeatifyConsole<wchar_t>(L"DNICC Client");
    NosLib::Console::InitializeModifiers::InitializeEventHandler();

    boost::asio::io_context io_context;

    boost::asio::ip::tcp::socket connectionSocket(io_context);

    try
    {
        (void)wprintf(L"(DEBUGGING) Type in hostname: ");
        std::string hostName;
        std::getline(std::cin, hostName);
        if (hostName.empty())
        {
            hostName = Constants::DefaultHostname;
        }

        /*
        Connects to the function using `resolver` which resolves the address e.g. (Noscka.com -> 123.123.123.123)
        Host - Hostname/Ip address
        Service - Service(Hostname for ports)/Port number
        */
        boost::asio::connect(connectionSocket, boost::asio::ip::tcp::resolver(io_context).resolve(hostName, Constants::DefaultPort));
        CentralLib::Logging::LogMessage<wchar_t>(L"Connected to server\n", true);

        switch (ClientLib::StartUp::GatherClientMode())
        {
        case ClientLib::StartUp::UserMode::Client:
        {
            CentralLib::Logging::LogMessage<wchar_t>(L"User Became Client\n", true);
            ClientLib::Client::StartClient(&io_context, &connectionSocket);
            break;
        }

        case ClientLib::StartUp::UserMode::Hosting:
        {
            CentralLib::Logging::LogMessage<wchar_t>(L"Client Hosting a Communications server\n", true);
            ClientLib::Hosting::StartHosting(&io_context, &connectionSocket);
            break;
        }
        }
    }
    catch (const std::exception& e)
    {
        CentralLib::Logging::LogMessage<wchar_t>(NosLib::String::ConvertString<wchar_t, char>(e.what()), true);
        std::wcerr << NosLib::String::ConvertString<wchar_t, char>(e.what()) << std::endl;
    }

    (void)wprintf(L"Press any button to continue"); _getch();
    return 0;
}