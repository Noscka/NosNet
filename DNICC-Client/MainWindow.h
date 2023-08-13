#pragma once
#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLineEdit>
#include "ui_MainWindow.h"

#include <Central/CentralLib.hpp>
#include <Central/Logging.hpp>
#include <Central/ServerEntry.hpp>

#include "Header/GlobalRoot.hpp"
#include "Header/Communication.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindowClass; };
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr) : QMainWindow(parent)
	{
		ui = new Ui::MainWindowClass();

		GlobalRoot::UI = ui;

		ui->setupUi(this);

		/*
		Connects to the function using `resolver` which resolves the address e.g. (Noscka.com -> 123.123.123.123)
		Host - Hostname/Ip address
		Service - Service(Hostname for ports)/Port number
		*/
		boost::asio::connect(*GlobalRoot::ConnectionSocket, boost::asio::ip::tcp::resolver(*GlobalRoot::IOContext).resolve(Constants::DefaultHostname, Constants::DefaultPort));
		CentralLib::Logging::CreateLog<wchar_t>(L"Connected to DCHLS server\n", false);

		/* receive server array from server to display in menu */
		ReceiveServerArray();

		/* MAIN PAGE */
		connect(ui->DirectButton, &QPushButton::released, this, [&]() { ui->MainStackedWidget->setCurrentIndex(1); ui->ConnectionStacked->setCurrentIndex(0); });
		connect(ui->GroupsButton, &QRadioButton::clicked, this, [&]() { ui->MainStackedWidget->setCurrentIndex(1); ui->ConnectionStacked->setCurrentIndex(1); });
		connect(ui->ServersButton, &QRadioButton::clicked, this, [&]() { ui->MainStackedWidget->setCurrentIndex(1); ui->ConnectionStacked->setCurrentIndex(2); });
		/* MAIN PAGE */

		/* FILL DIFFERENT CONNECTION PAGES */
		for (CentralLib::ServerEntry* entry : CentralLib::ServerEntry::ServerRegistry)
		{
			switch (entry->GetServerType())
			{
			case CentralLib::ServerEntry::enServerType::Direct:
				ui->DirectChoiceScrollArea->AddServerEntry(entry);
				break;
			case CentralLib::ServerEntry::enServerType::Group:
				ui->GroupsChoiceScrollArea->AddServerEntry(entry);
				break;
			case CentralLib::ServerEntry::enServerType::Dedicated:
				ui->DedicatedChoiceScrollArea->AddServerEntry(entry);
				break;
			}
		}
		/* FILL DIFFERENT CONNECTION PAGES */
	}

    ~MainWindow()
	{
		delete ui;
	}
protected:
	void ReceiveServerArray()
	{
		/* Aliased with using StrippedClientTracker */
		using AliasedClientReponse = ClientLib::Communications::ClientResponse;

		/* Tell server which path going down */
		AliasedClientReponse::CreateSerializeSend(GlobalRoot::ConnectionSocket, AliasedClientReponse::InformationCodes::RequestServerArray, L"requesting server array");

		/* Receive array */
		boost::asio::streambuf ContentBuffer;
		boost::asio::read_until((*GlobalRoot::ConnectionSocket), ContentBuffer, Definition::Delimiter);
		CentralLib::ServerEntry::DeserializeArray(&ContentBuffer);
	}

private:
    Ui::MainWindowClass* ui;
};
