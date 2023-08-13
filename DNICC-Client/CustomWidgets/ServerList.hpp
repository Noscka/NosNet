#pragma once
#include <QCoreApplication>
#include <QtWidgets\QLayout>
#include <QtWidgets\QScrollBar>
#include <QtWidgets/QScrollArea>
#include <QtWidgets\QLabel>
#include <QPalette>
#include "Central\ServerEntry.hpp"


/*
implement button from here
for (int i = 0; i <= ClientArray.GetLastArrayIndex(); i++)
				{
					CentralLib::ClientInterfacing::StrippedClientTracker* currentEntry = ClientArray[i];

					QPushButton* serverEntry = new QPushButton();
					std::wstring entryName = std::format(L"IP: {}", NosLib::String::ToWstring(currentEntry->ReturnIPAddress()));
					serverEntry->setObjectName(entryName);
					serverEntry->setText(QString::fromStdWString(entryName));
					QMainWindow::connect(serverEntry, &QPushButton::released, [currentEntry]()
					{
						ClientLib::Client::InitiateJoiningHost(currentEntry);
					});

					ui->ServerSelectionVerticalLayout->addWidget(serverEntry);
				}
*/

class ServerList : public QScrollArea
{
	Q_OBJECT
protected:
	QVBoxLayout* ServerListLayout;
	QWidget* ServerListWidget;

public:
	ServerList(QWidget* parent = nullptr) : QScrollArea(parent)
	{
		ServerListWidget = new QWidget(this);
		setWidget(ServerListWidget);

		ServerListLayout = new QVBoxLayout(ServerListWidget);
		ServerListLayout->setContentsMargins(0, 0, 0, 0);
		ServerListLayout->setAlignment(Qt::AlignmentFlag::AlignTop);
		ServerListWidget->setLayout(ServerListLayout);

		QCoreApplication::processEvents();
	}

	void AddServerEntry(/* const */ CentralLib::ServerEntry* server)
	{
		/* Create message object */
		QWidget* serverContainer = new QWidget(this);
		serverContainer->setAutoFillBackground(true);
		serverContainer->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));

		QHBoxLayout* serverLayout = new QHBoxLayout();
		serverLayout->setContentsMargins(6, 6, 6, 6);
		serverContainer->setLayout(serverLayout);

		QLabel* serverName = new QLabel();
		serverName->setText(QString::fromStdWString(server->GetServerName()));
		QFont usernameFont = serverName->font();
		usernameFont.setPointSize(16);
		serverName->setFont(usernameFont);
		serverLayout->addWidget(serverName);

		QLabel* serverStatus = new QLabel();
		serverStatus->setText(QString::fromStdWString(L"IMPLEMENT SERVER STATUS AS STRING"));
		serverLayout->addWidget(serverStatus);

		QLabel* serverIPAddress = new QLabel();
		serverIPAddress->setText(QString::fromStdString(server->GetIpAddressAsString()));
		serverLayout->addWidget(serverIPAddress);
		/* Create message object */

		this->widget()->layout()->addWidget(serverContainer);

		this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
		QCoreApplication::processEvents();
	}
};