#pragma once
#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLineEdit>
#include "ui_MainWindow.h"

#include "Central/CentralLib.hpp"
#include "Central/Logging.hpp"

#include "Header/ClientLib.hpp"
#include "Header/Client.hpp"
#include "Header/Host.hpp"

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
		/* START PAGE */
		connect(ui->HostNameEntry, &QPushButton::released, this, &MainWindow::ProcessUserInput);

		connect(ui->NormalSelection, &QRadioButton::clicked, this, [&]() { ui->HostNameEntry->setEnabled(true); });
		connect(ui->HostSelection, &QRadioButton::clicked, this, [&]() { ui->HostNameEntry->setEnabled(true); });
		/* START PAGE */

		/* LOGIN PAGE */
		connect(ui->LoginTextbox, &QLineEdit::textChanged, this, [&]()
		{
			/* if no text or more then 30, blackout box */
			if (ui->LoginTextbox->text().length() <= 0 || ui->LoginTextbox->text().length() > 30)
			{
				ui->LoginButton->setEnabled(false);
				return;
			}

			ui->LoginButton->setEnabled(true);
		});

		connect(ui->LoginButton, &QPushButton::released, &ClientLib::Client::ValidateUsername);
		/* LOGIN PAGE */
	}

    ~MainWindow()
	{
		delete ui;
	}

protected:
	void ProcessUserInput()
	{
		/*
		Connects to the function using `resolver` which resolves the address e.g. (Noscka.com -> 123.123.123.123)
		Host - Hostname/Ip address
		Service - Service(Hostname for ports)/Port number
		*/
		boost::asio::connect((*ConnectionSocket), boost::asio::ip::tcp::resolver(*IOContext).resolve(ui->HostNameText->text().toStdString(), Constants::DefaultPort));
		CentralLib::Logging::CreateLog<wchar_t>(L"Connected to DCHLS server\n", false);

		switch (ClientLib::StartUp::GatherClientMode(ui))
		{
		case ClientLib::StartUp::UserMode::Client:
		{
			CentralLib::Logging::CreateLog<wchar_t>(L"User Became Client\n", false);
			ClientLib::Client::StartClient(ui, IOContext, ConnectionSocket);
			break;
		}

		case ClientLib::StartUp::UserMode::Hosting:
		{
			CentralLib::Logging::CreateLog<wchar_t>(L"Client Hosting a Communications server\n", false);
			ClientLib::Hosting::StartHosting(IOContext, ConnectionSocket);
			break;
		}
		}
	}

private:
    Ui::MainWindowClass* ui;
	boost::asio::io_context* IOContext;
	boost::asio::ip::tcp::socket* ConnectionSocket;
};
