#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>

#include <QtWidgets/QApplication>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
	boost::asio::io_context io_context;
	boost::asio::ip::tcp::socket connectionSocket(io_context);

    QApplication a(argc, argv);
    QApplication::setStyle("Fusion");
    MainWindow w(&io_context, &connectionSocket);
    w.show();
    return a.exec();
}
