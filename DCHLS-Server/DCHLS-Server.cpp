#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <NosStdLib/Console.hpp>
#include <NosStdLib/DynamicArray.hpp>
#include <NosStdLib/String.hpp>

#include <Central/CentralLib.hpp>
#include <Server/ServerLib.hpp>

#include <iostream>
#include <conio.h>

class tcp_connection_handle
{
private:
    boost::asio::ip::tcp::socket ConnectionSocket;
    ServerLib::ClientManagement::ClientTracker* ClientTrackerAttached;

    tcp_connection_handle(boost::asio::io_context& io_context) : ConnectionSocket(io_context) {}

    ~tcp_connection_handle()
    {
        delete ClientTrackerAttached;
    }
public:
    static tcp_connection_handle* create(boost::asio::io_context& io_context) { return new tcp_connection_handle(io_context); }

    boost::asio::ip::tcp::socket& GetSocket()
    { 
        return ConnectionSocket;
    }

    void start()
    {
        wprintf(std::format(L"Client Connected from {}\n", CentralLib::ReturnAddress(ConnectionSocket.remote_endpoint())).c_str());

        wprintf(L"Creating profile and adding to array\n");

        /* Create ClientTracker Object and attach it to current session */
        
        ClientTrackerAttached = ServerLib::ClientManagement::ClientTracker::RegisterClient(L"Default name !!CHANGE!!", CentralLib::ClientInterfacing::StrippedClientTracker::ClientStatus::Online, &ConnectionSocket);

        try
		{
            wprintf(CentralLib::ClientInterfacing::StrippedClientTracker::ListClientArray().c_str());

			boost::asio::streambuf streamBuffer;
            CentralLib::ClientInterfacing::StrippedClientTracker::SerializeArray(&streamBuffer, *(ClientTrackerAttached->GetArrayPositionPointer()));
			boost::asio::write(ConnectionSocket, streamBuffer);
			boost::asio::write(ConnectionSocket, boost::asio::buffer(Definition::Delimiter));
        }
        catch (const std::exception& e)
        {
            std::wcerr << NosStdLib::String::ConvertString<wchar_t, char>(e.what()) << std::endl;
        }

        wprintf(std::format(L"Connection with {} Terminated\n", CentralLib::ReturnAddress(ConnectionSocket.remote_endpoint())).c_str());
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
        boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 58233));

        wprintf(L"Server started\n");

        while (true)
        {
            /* tcp_connection object which allows for managed of multiple users */
            tcp_connection_handle* newConSim = tcp_connection_handle::create(io_context);

            boost::system::error_code error;

            /* accept incoming connection and assigned it to the tcp_connection object socket */
            acceptor.accept(newConSim->GetSocket(), error);

            /* if no errors, create thread for the new connection */
            if (!error) { boost::thread* ClientThread = new boost::thread(boost::bind(&tcp_connection_handle::start, newConSim)); }
        }
    }
    catch (const std::exception& e)
    {
        std::wcerr << NosStdLib::String::ConvertString<wchar_t, char>(e.what()) << std::endl;
    }

    wprintf(L"Press any button to continue"); _getch();
    return 0;
}