#pragma once
#include <QCoreApplication>
#include <QtWidgets\QLayout>
#include <QtWidgets\QScrollBar>
#include <QtWidgets/QScrollArea>
#include <QtWidgets\QLabel>
#include <QPalette>
#include "..\Header\ServerManager.hpp"

class ClientList : public QScrollArea
{
	Q_OBJECT
protected:
	QVBoxLayout* ChatFeedLayout;
	QWidget* ChatFeedWidget;

public slots:
	void AddNewServerEntry(ServerLib::ServerManager* newServerEntry)
	{
		AddServerEntry(newServerEntry);
	}

public:
	ClientList(QWidget* parent = nullptr) : QScrollArea(parent)
	{
		ChatFeedWidget = new QWidget(this);
		setWidget(ChatFeedWidget);

		ChatFeedLayout = new QVBoxLayout(ChatFeedWidget);
		ChatFeedLayout->setContentsMargins(0, 0, 0, 0);
		ChatFeedLayout->setAlignment(Qt::AlignmentFlag::AlignTop);
		ChatFeedWidget->setLayout(ChatFeedLayout);

		QCoreApplication::processEvents();
	}

	void AddServerEntry(ServerLib::ServerManager* newServerEntry)
	{
		/* Create message object */
		QWidget* clientContainer = new QWidget(this);
		clientContainer->setAutoFillBackground(true);
		clientContainer->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));

		QHBoxLayout* clientLayout = new QHBoxLayout();
		clientLayout->setContentsMargins(6, 6, 6, 6);
		clientContainer->setLayout(clientLayout);

		QLabel* username = new QLabel();
		username->setText(QString::fromStdWString(newServerEntry->GetServerName()));
		QFont usernameFont = username->font();
		usernameFont.setPointSize(16);
		username->setFont(usernameFont);
		clientLayout->addWidget(username);

		QLabel* userStatus = new QLabel();
		userStatus->setText(QString::fromStdWString(L"IMPLEMENT SERVER TYPE AS STRING"));
		clientLayout->addWidget(userStatus);

		QLabel* ipAddress = new QLabel();
		ipAddress->setText(QString::fromStdString(newServerEntry->GetIpAddressAsString()));
		clientLayout->addWidget(ipAddress);
		/* Create message object */

		this->widget()->layout()->addWidget(clientContainer);

		this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
		QCoreApplication::processEvents();
	}
};