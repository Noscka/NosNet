#pragma once
#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindowClass; };
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	MainWindow(boost::asio::io_context* ioContext, boost::asio::ip::tcp::socket* connectionSocket, QWidget* parent = nullptr) : QMainWindow(parent)
	{
		ui = new Ui::MainWindowClass();

		IOContext = ioContext;
		ConnectionSocket = connectionSocket;

		ui->setupUi(this);
		connect(ui->HostNameEntry, &QPushButton::released, this, &MainWindow::ProcessUserInput);
	}

    ~MainWindow()
	{
		delete ui;
	}

protected:
	void ProcessUserInput()
	{

	}

private:
    Ui::MainWindowClass* ui;
	boost::asio::io_context* IOContext;
	boost::asio::ip::tcp::socket* ConnectionSocket;
};
