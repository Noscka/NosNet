#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>

#include <QtWidgets/QApplication>

#include "Header/GlobalRoot.hpp"

#include "MainWindow.h"

int main(int argc, char *argv[])
{
	boost::asio::io_context ioContext;
	boost::asio::ip::tcp::socket connectionSocket(ioContext);

    GlobalRoot::IOContext = &ioContext;
    GlobalRoot::ConnectionSocket = &connectionSocket;

    QApplication a(argc, argv);
    GlobalRoot::AppPointer = &a;
    //QApplication::setStyle("Fusion");
    MainWindow w;
    w.show();
    return a.exec();
}
