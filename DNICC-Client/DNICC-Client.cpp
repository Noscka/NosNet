#include <boost/asio.hpp>

#include <NosStdLib/Console.hpp>

#include <iostream>
#include <conio.h>

int main()
{
    NosStdLib::Console::InitializeModifiers::EnableUnicode();
    NosStdLib::Console::InitializeModifiers::EnableANSI();
    NosStdLib::Console::InitializeModifiers::BeatifyConsole<wchar_t>(L"DNIC Client");
    NosStdLib::Console::InitializeModifiers::InitializeEventHandler();

    boost::asio::io_context io_context;

    boost::asio::ip::tcp::socket socket(io_context);

    try
    {
        wprintf(L"Type in hostname: ");
        std::string HostName;
        std::getline(std::cin, HostName);

        /*
        Connects to the function using `resolver` which resolves the address e.g. (Noscka.com -> 123.123.123.123)
        Host - Hostname/Ip address
        Service - Service(Hostname for ports)/Port number
        */
        boost::asio::connect(socket, boost::asio::ip::tcp::resolver(io_context).resolve(HostName, "58233"));

        wprintf(L"Connected to server\n");

        
    }
    catch (std::exception& e)
    {
        std::wcerr << e.what() << std::endl;
    }

    wprintf(L"Press any button to continue"); _getch();
    return 0;
}