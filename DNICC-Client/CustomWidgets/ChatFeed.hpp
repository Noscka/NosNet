#pragma once
#include <QCoreApplication>
#include <QtWidgets\QLayout>
#include <QtWidgets\QScrollBar>
#include <QtWidgets/QScrollArea>
#include <QtWidgets\QLabel>
#include <QPalette>

#include "..\Header\Communication.hpp"
#include "..\Header\ClientManagement\ClientManager.hpp"

#include <format>

class ChatFeed : public QScrollArea
{
	Q_OBJECT
protected:
	QVBoxLayout* ChatFeedLayout;
	QWidget* ChatFeedWidget;

public slots:
	void ReceiveMessage(Communications::MessageObject receivedMessage)
	{
		NewMessage(receivedMessage);
	}

	void ClientConnected(ClientManager* connectedClient)
	{
		Communications::MessageObject tempMessageObject(connectedClient, std::format(L"{} connected", connectedClient->GetClientName()));
		NewMessage(tempMessageObject);
	}


public:
	ChatFeed(QWidget* parent = nullptr) : QScrollArea(parent)
	{
		ChatFeedWidget = new QWidget(this);
		setWidget(ChatFeedWidget);

		ChatFeedLayout = new QVBoxLayout(ChatFeedWidget);
		ChatFeedLayout->setContentsMargins(0, 0, 0, 0);
		ChatFeedLayout->setAlignment(Qt::AlignmentFlag::AlignTop);
		ChatFeedWidget->setLayout(ChatFeedLayout);

		QCoreApplication::processEvents();
	}

	void NewMessage(Communications::MessageObject& receivedMessage)
	{
		/* Create message object */
		QWidget* messageContainer = new QWidget(this);
		messageContainer->setAutoFillBackground(true);
		messageContainer->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));

		QVBoxLayout* messageLayout = new QVBoxLayout();
		messageLayout->setContentsMargins(6, 6, 6, 6);
		messageContainer->setLayout(messageLayout);

		QLabel* username = new QLabel();
		username->setText(QString::fromStdWString(receivedMessage.GetUserInfo()->GetClientName()));
		QFont usernameFont = username->font();
		usernameFont.setPointSize(16);
		username->setFont(usernameFont);
		messageLayout->addWidget(username);

		QLabel* message = new QLabel();
		message->setText(QString::fromStdWString(receivedMessage.GetMessage()));
		messageLayout->addWidget(message);
		/* Create message object */
		
		this->widget()->layout()->addWidget(messageContainer);

		//ChatFeedLayout->addWidget(messageContainer);
		this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
		QCoreApplication::processEvents();
	}
};