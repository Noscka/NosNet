#pragma once
#include <QCoreApplication>
#include <QtWidgets\QLayout>
#include <QtWidgets\QScrollBar>
#include <QtWidgets/QScrollArea>
#include <QtWidgets\QLabel>
#include <QPalette>
#include "..\Header\ClientManagement\ClientManager.hpp"

class ClientList : public QScrollArea
{
	Q_OBJECT
public slots:
	void ClientConnected(ClientLib::ClientManager* connectedClient)
	{
		AddClientEntry(connectedClient);
	}

protected:
	QVBoxLayout* ClientListLayout;
	QWidget* ClientListWidget;

public:
	ClientList(QWidget* parent = nullptr) : QScrollArea(parent)
	{
		ClientListWidget = new QWidget(this);
		setWidget(ClientListWidget);

		ClientListLayout = new QVBoxLayout(ClientListWidget);
		ClientListLayout->setContentsMargins(0, 0, 0, 0);
		ClientListLayout->setAlignment(Qt::AlignmentFlag::AlignTop);
		ClientListWidget->setLayout(ClientListLayout);

		QCoreApplication::processEvents();
	}

	void AddClientEntry(ClientLib::ClientManager* client)
	{
		/* Create message object */
		QWidget* clientContainer = new QWidget(this);
		clientContainer->setAutoFillBackground(true);
		clientContainer->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));

		QHBoxLayout* clientLayout = new QHBoxLayout();
		clientLayout->setContentsMargins(6, 6, 6, 6);
		clientContainer->setLayout(clientLayout);

		QLabel* username = new QLabel();
		username->setText(QString::fromStdWString(client->GetClientName()));
		QFont usernameFont = username->font();
		usernameFont.setPointSize(16);
		username->setFont(usernameFont);
		clientLayout->addWidget(username);

		QLabel* userStatus = new QLabel();
		userStatus->setText(QString::fromStdWString(L"IMPLEMENT CLIENT STATUS AS STRING"));
		clientLayout->addWidget(userStatus);

		QLabel* ipAddress = new QLabel();
		ipAddress->setText(QString::fromStdString(client->GetIpAddressAsString()));
		clientLayout->addWidget(ipAddress);
		/* Create message object */

		this->widget()->layout()->addWidget(clientContainer);

		this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
		QCoreApplication::processEvents();
	}
};