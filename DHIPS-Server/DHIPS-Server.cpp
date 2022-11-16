#include <NosStdLib/Global.hpp>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <iostream>
#include <conio.h>

class tcp_connection_handle
{
private:
    boost::asio::ip::tcp::socket connectionSocket;

    tcp_connection_handle(boost::asio::io_context& io_context) : connectionSocket(io_context) {}
public:
    static tcp_connection_handle* create(boost::asio::io_context& io_context) { return new tcp_connection_handle(io_context); }

    boost::asio::ip::tcp::socket& GetSocket()
    { 
        return connectionSocket;
    }

    void start()
    {
        //wprintf(std::format(L"Client Connected from {}\n", GlobalFunction::ReturnAddress(socket.local_endpoint())).c_str());

        try
        {

        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
};

int main()
{
    NosStdLib::Global::Console::InitializeModifiers::EnableUnicode();
    NosStdLib::Global::Console::InitializeModifiers::EnableANSI();

    try
    {
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 58233));

        //SetConsoleTitle(std::wstring(L"File Server at " + GlobalFunction::ReturnAddress(acceptor.local_endpoint())).c_str());

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
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    wprintf(L"Press any button to continue"); _getch();
    return 0;
}