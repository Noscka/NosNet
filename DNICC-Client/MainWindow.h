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
#include "Header/Host/DirectHost.hpp"
#include "Header/Client/Connect.hpp"

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
		connect(ui->DirectButton, &QPushButton::released, this, [&]() { ui->MainStackedWidget->setCurrentIndex(0); ui->ConnectionStacked->setCurrentIndex(0); });
		connect(ui->GroupsButton, &QPushButton::released, this, [&]() { ui->MainStackedWidget->setCurrentIndex(0); ui->ConnectionStacked->setCurrentIndex(1); });
		connect(ui->ServersButton, &QPushButton::released, this, [&]() { ui->MainStackedWidget->setCurrentIndex(0); ui->ConnectionStacked->setCurrentIndex(2); });
		/* MAIN PAGE */

		/* CONNECTION PAGES */
		connect(ui->DirectCreateButton, &QPushButton::released, this, [&]() { ui->MainStackedWidget->setCurrentIndex(2); ui->CreationTabWidget->setCurrentIndex(0); });
		connect(ui->GroupsCreateButton, &QPushButton::released, this, [&]() { ui->MainStackedWidget->setCurrentIndex(2); ui->CreationTabWidget->setCurrentIndex(1); });
		connect(ui->DedicatedCreateButton, &QPushButton::released, this, [&]() { ui->MainStackedWidget->setCurrentIndex(2); ui->CreationTabWidget->setCurrentIndex(2); });
		/* CONNECTION PAGES */

		/* CREATION PAGES */
		connect(ui->CreateDirectButton, &QPushButton::released, &ClientLib::DirectHost::InitializeDirectHosting);
		connect(ui->CreateDirectLineEdit, &QLineEdit::textChanged, this, [&]()
		{
			std::wstring username;

			GlobalRoot::UI->CreateDirectLineEdit->text().toStdWString();

			bool validation = CentralLib::Validation::ValidateUsername(username);

			ui->CreateDirectButton->setEnabled(validation);
		});

		connect(ui->CreateGroupButton, &QPushButton::released, this, [&]() {/* implement */ });
		connect(ui->CreateGroupLineEdit, &QLineEdit::textChanged, this, [&]()
		{
			ui->CreateGroupButton->setEnabled(CentralLib::Validation::ValidateUsername(GlobalRoot::UI->CreateGroupLineEdit->text().toStdWString()));
		});
		/* CREATION PAGES */

		/* CHAT LOGIN PAGE */
		connect(ui->LoginButton, &QPushButton::released, this, [&]() {/* implement */ });
		connect(ui->LoginLineEdit, &QLineEdit::textChanged, this, [&]()
		{
			/* validate username using text from line edit, enable the login button depending on if the username is valid */
			ui->LoginButton->setEnabled(CentralLib::Validation::ValidateUsername(GlobalRoot::UI->LoginLineEdit->text().toStdWString()));
		});
		/* CHAT LOGIN PAGE */

		/* CHAT PAGE */
		connect(ui->ChatTextBar, &ChatLineEdit::SentMessage, ui->ChatFeedScroll, &ChatFeed::ReceiveMessage);
		/* CHAT PAGE */

		/* FILL DIFFERENT CONNECTION PAGES */
		for (CentralLib::ServerEntry* entry : CentralLib::ServerEntry::ServerRegistry)
		{
			switch (entry->GetServerType())
			{
			case CentralLib::ServerEntry::enServerType::Direct:
				ui->DirectChoiceScrollArea->AddServerEntry(entry, &ClientLib::Connect::ServerConnect);
				break;
			case CentralLib::ServerEntry::enServerType::Group:
				ui->GroupsChoiceScrollArea->AddServerEntry(entry, &ClientLib::Connect::ServerConnect);
				break;
			case CentralLib::ServerEntry::enServerType::Dedicated:
				ui->DedicatedChoiceScrollArea->AddServerEntry(entry, &ClientLib::Connect::ServerConnect);
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
		CentralLib::Logging::CreateLog<wchar_t>(L"Received Server Registry from DCHLS\n", false);

		/* Disconnect from DCHLS server */
		GlobalRoot::ConnectionSocket->cancel();
		CentralLib::Logging::CreateLog<wchar_t>(L"Disconnected from DCHLS\n", false);
	}

private:
    Ui::MainWindowClass* ui;
};
