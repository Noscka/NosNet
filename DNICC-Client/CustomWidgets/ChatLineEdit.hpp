#pragma once

#include <QCoreApplication>
#include <QtWidgets\QLayout>
#include <QtWidgets\QLineEdit>
#include <QKeyEvent>

#include "..\Header\Client\SendReceive.hpp"
#include "..\Header\Client\ClientRoot.hpp"

class ChatLineEdit : public QLineEdit
{
	Q_OBJECT
signals:
	void SentMessage(ClientLib::Communications::MessageObject sentMessage);

public:
	ChatLineEdit(QWidget* parent = nullptr) : QLineEdit(parent)
	{

	}
protected:
	void keyPressEvent(QKeyEvent* keyEvent) override
	{
		if (keyEvent->key() != Qt::Key_Return)
		{
			QLineEdit::keyPressEvent(keyEvent);
			return;
		}

		ClientLib::Communications::MessageObject messageObject(ClientLib::Client::clientObject, text().toStdWString());

		/* if enter key, then send the message */
		emit SentMessage(messageObject);
		ClientLib::Client::SendMessage(messageObject.GetMessage());

		setText(QStringLiteral(""));
	}
};