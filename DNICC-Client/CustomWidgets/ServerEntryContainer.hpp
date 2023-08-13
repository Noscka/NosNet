#pragma once
#include <QCoreApplication>
#include <QtWidgets\QLayout>
#include <QtWidgets\QLabel>
#include <QWidget>

#include <Central\ServerEntry.hpp>

class ServerEntryContainer : public QWidget
{
	Q_OBJECT
signals:
	void MouseReleased(CentralLib::ServerEntry* server);
protected:
	QHBoxLayout* serverContainerLayout;
	QLabel* ServerName;
	QLabel* ServerStatus;
	QLabel* ServerIPAddress;

	CentralLib::ServerEntry* Server;
public:
	ServerEntryContainer(CentralLib::ServerEntry* server, QWidget* parent = nullptr) : QWidget(parent)
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