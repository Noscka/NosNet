#pragma once

#include <QCoreApplication>
#include <QtWidgets\QLayout>
#include <QtWidgets\QLineEdit>
#include <QKeyEvent>

#include "..\Header\Client\SendReceive.hpp"

class ChatLineEdit : public QLineEdit
{
	Q_OBJECT

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
		}

		/* if enter key, then send the message */
		ClientLib::Client::SendMessage(text().toStdWString());
	}
};