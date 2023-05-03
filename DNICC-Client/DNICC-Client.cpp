#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>

#include <NosStdLib/Console.hpp>
#include <NosStdLib/String.hpp>

#include <Central/CentralLib.hpp>
#include <Client/ClientLib.hpp>

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

        boost::asio::streambuf ContentBuffer;

        boost::asio::read_until(socket, ContentBuffer, Definition::Delimiter);

        CentralLib::ClientInterfacing::StrippedClientTracker ClientTrackerAttached;


        wprintf(L"Connected to server\n");

        
    }
    catch (const std::exception& e)
    {
        std::wcerr << NosStdLib::String::ConvertString<wchar_t, char>(e.what()) << std::endl;
    }

    wprintf(L"Press any button to continue"); _getch();
    return 0;
}