#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <NosStdLib/Console.hpp>
#include <NosStdLib/DynamicArray.hpp>
#include <NosStdLib/String.hpp>

#include <Central/CentralLib.hpp>
#include <Central/Logging.hpp>
#include "Header/ServerLib.hpp"
#include "Header/ServerConnectionProcessing.hpp"

#include <iostream>
#include <conio.h>

class tcp_connection_handle
{
private:
    boost::asio::ip::tcp::socket ConnectionSocket;
    CentralLib::ClientManagement::ClientTracker* ClientTrackerAttached;

    tcp_connection_handle(boost::asio::io_context& io_context) : ConnectionSocket(io_context) {}

    ~tcp_connection_handle()
    {
        //delete ClientTrackerAttached; /* COMMENTED OUT FOR DEBUGGING */
    }

    void StartImp()
    {
        CentralLib::Logging::LogMessage<wchar_t>(std::format(L"Client Connected from {}\n", CentralLib::ReturnAddress(ConnectionSocket.remote_endpoint())), true);

        CentralLib::Logging::LogMessage<wchar_t>(L"Creating profile and adding to array\n", true);

        try
        {
            /* Check what path the user takes */
            boost::asio::streambuf clientReponseBuffer;
            boost::asio::read_until(ConnectionSocket, clientReponseBuffer, Definition::Delimiter);

            CentralLib::Communications::CentralizedClientResponse clientReponse(&clientReponseBuffer);

            switch (clientReponse.GetInformationCode())
            {
            case CentralLib::Communications::CentralizedClientResponse::InformationCodes::GoingClientPath:
                CentralLib::Logging::LogMessage<wchar_t>(L"User Became Client\n", true);
                ServerLib::Processing::UserClientPath(&ConnectionSocket, ClientTrackerAttached);
                break;

            case CentralLib::Communications::CentralizedClientResponse::InformationCodes::GoingHostingPath:
                CentralLib::Logging::LogMessage<wchar_t>(L"Client is hosting a communications server\n", true);
                ServerLib::Processing::UserHostPath(&ConnectionSocket, ClientTrackerAttached);
                break;
            }
        }
        catch (const std::exception& e)
        {
            CentralLib::Logging::LogMessage<wchar_t>(NosStdLib::String::ConvertString<wchar_t, char>(e.what()), true);
            std::wcerr << NosStdLib::String::ConvertString<wchar_t, char>(e.what()) << std::endl;
        }

        (void)wprintf(std::format(L"Connection with {} Terminated\n", CentralLib::ReturnAddress(ConnectionSocket.remote_endpoint())).c_str());
    }
public:
    static tcp_connection_handle* create(boost::asio::io_context& io_context) { return new tcp_connection_handle(io_context); }

    boost::asio::ip::tcp::socket& GetSocket()
    { 
        return ConnectionSocket;
    }

    void StartPublic()
    {
        StartImp(); /* Run function and make sure it gets deleted */
        delete this; /* commit suicide if the connection ends as the object won't get used/deleted otherwise */
    }
};

int main()
{
    NosStdLib::Console::InitializeModifiers::EnableUnicode();
    NosStdLib::Console::InitializeModifiers::EnableANSI();
    NosStdLib::Console::InitializeModifiers::BeatifyConsole<wchar_t>(L"DCHLS Server");
    NosStdLib::Console::InitializeModifiers::InitializeEventHandler();

    try
    {
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), std::stoi(Constants::DefaultPort)));

        wprintf(L"Server started\n");

        while (true)
        {
            /* tcp_connection object which allows for managed of multiple users */
            tcp_connection_handle* newConSim = tcp_connection_handle::create(io_context);

            boost::system::error_code error;

            /* accept incoming connection and assigned it to the tcp_connection object socket */
            acceptor.accept(newConSim->GetSocket(), error);

            /* if no errors, create thread for the new connection */
            if (!error) { boost::thread* ClientThread = new boost::thread(boost::bind(&tcp_connection_handle::StartPublic, newConSim)); }
        }
    }
    catch (const std::exception& e)
    {
        std::wcerr << NosStdLib::String::ConvertString<wchar_t, char>(e.what()) << std::endl;
    }

    (void)wprintf(L"Press any button to continue"); _getch();
    return 0;
}