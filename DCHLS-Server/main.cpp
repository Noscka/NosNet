#include "MainWindow.h"
#include <QtWidgets/QApplication>

#include "Header/GlobalRoot.hpp"

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
