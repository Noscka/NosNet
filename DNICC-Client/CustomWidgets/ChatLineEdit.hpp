#pragma once

#include <QCoreApplication>
#include <QtWidgets\QLayout>
#include <QtWidgets\QLineEdit>
#include <qdrag.h>
#include <qmimedata.h>
#include <QKeyEvent>

#include "..\Header\Client\SendReceive.hpp"
#include "..\Header\GlobalRoot\GlobalRoot_Int.hpp"
#include "..\Header\GlobalRoot\GlobalRoot_Ext.hpp"

class ChatLineEdit : public QLineEdit
{
	Q_OBJECT
signals:
	void SentMessage(Communications::MessageObject sentMessage);

public:
	ChatLineEdit(QWidget* parent = nullptr) : QLineEdit(parent)
	{
		setAcceptDrops(true);
	}
protected:
	void dragEnterEvent(QDragEnterEvent* e) override
	{
		if (e->mimeData()->hasUrls())
		{
			e->acceptProposedAction();
		}
	}

	void dropEvent(QDropEvent* e) override
	{
		foreach(const QUrl & url, e->mimeData()->urls())
		{
			QString fileName = url.toLocalFile();
			qDebug() << "Dropped file:" << fileName;
		}
	}

	void keyPressEvent(QKeyEvent* keyEvent) override
	{
		if (keyEvent->key() != Qt::Key_Return)
		{
			QLineEdit::keyPressEvent(keyEvent);
			return;
		}

		Communications::MessageObject messageObject(GlobalRoot::ThisClient, text().toStdWString());

		/* if enter key, then send the message */
		emit SentMessage(messageObject);
		SendReceive::ChatSend::SendMessage(messageObject.GetMessage());

		setText(QStringLiteral(""));
	}
};