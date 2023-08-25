#pragma once
#include <QCoreApplication>
#include <QtWidgets\QLayout>
#include <QtWidgets\QLabel>
#include <QWidget>

#include "..\Header\ServerManager.hpp"

class ClientList;

class ServerEntryContainer : public QWidget
{
	Q_OBJECT
signals:
	void MouseReleased(ServerManager* server);
protected:
	friend ClientList;

	QHBoxLayout* serverContainerLayout;
	QLabel* ServerName;
	QLabel* ServerStatus;
	QLabel* ServerIPAddress;

	ServerManager* Server;
public:
	ServerEntryContainer(ServerManager* server, QWidget* parent = nullptr) : QWidget(parent)
	{
		Server = server;

		setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
		serverContainerLayout = new QHBoxLayout();
		serverContainerLayout->setContentsMargins(6, 6, 6, 6);
		setLayout(serverContainerLayout);

		ServerName = new QLabel();
		ServerName->setText(QString::fromStdWString(server->GetServerName()));
		QFont usernameFont = ServerName->font();
		usernameFont.setPointSize(16);
		ServerName->setFont(usernameFont);
		serverContainerLayout->addWidget(ServerName);

		ServerStatus = new QLabel();
		ServerStatus->setText(QString::fromStdWString(L"IMPLEMENT SERVER STATUS AS STRING"));
		serverContainerLayout->addWidget(ServerStatus);

		ServerIPAddress = new QLabel();
		ServerIPAddress->setText(QString::fromStdWString(server->GetIpAddressAsWString()));
		serverContainerLayout->addWidget(ServerIPAddress);
	}

protected:
	void mouseReleaseEvent(QMouseEvent* event) override
	{
		emit MouseReleased(Server);
	}
};