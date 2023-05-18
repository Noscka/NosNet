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
        ClientTrackerAttached->ChangeStatus(CentralLib::ClientInterfacing::StrippedClientTracker::ClientStatus::Offline);

        //delete ClientTrackerAttached; /* COMMENTED OUT FOR DEBUGGING */
    }
public:
    static tcp_connection_handle* create(boost::asio::io_context& io_context) { return new tcp_connection_handle(io_context); }

    boost::asio::ip::tcp::socket& GetSocket()
    { 
        return ConnectionSocket;
    }

    void start()
    {
        CentralLib::Logging::LogMessage<wchar_t>(std::format(L"Client Connected from {}\n", CentralLib::ReturnAddress(ConnectionSocket.remote_endpoint())), true);

        CentralLib::Logging::LogMessage<wchar_t>(L"Creating profile and adding to array\n", true);

        try
		{
            bool initialValidation = true;
            while(initialValidation)
            { /* Scoped to delete usernameBuffer after use */
                /* Get Username from Client */
				boost::asio::streambuf usernameBuffer;
				size_t lenght = boost::asio::read_until(ConnectionSocket, usernameBuffer, Definition::Delimiter);

                std::wstring clientsUsername = CentralLib::streamBufferToWstring(&usernameBuffer, lenght);

				boost::asio::streambuf responseBuffer;

                if (CentralLib::Validation::ValidateUsername(clientsUsername)) /* username is valid */
                {
					/* Create ClientTracker Object and attach it to current session */
					ClientTrackerAttached = ServerLib::ClientManagement::ClientTracker::RegisterClient(clientsUsername, CentralLib::ClientInterfacing::StrippedClientTracker::ClientStatus::Online, &ConnectionSocket);
                    initialValidation = false;
					ServerLib::Communications::ServerResponse(CentralLib::Communications::CentralizedServerResponse::InformationCodes::Accepted, L"server accepted username").serializeObject(&responseBuffer);
                }
                else /* username isn't valid */
                {
					ServerLib::Communications::ServerResponse(CentralLib::Communications::CentralizedServerResponse::InformationCodes::NotAccepted, L"server didn't accept username").serializeObject(&responseBuffer);
                }

				boost::asio::write(ConnectionSocket, responseBuffer);
				boost::asio::write(ConnectionSocket, boost::asio::buffer(Definition::Delimiter));
            }
            
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