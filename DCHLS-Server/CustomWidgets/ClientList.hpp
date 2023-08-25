#pragma once
#include <QCoreApplication>
#include <QtWidgets\QLayout>
#include <QtWidgets\QScrollBar>
#include <QtWidgets/QScrollArea>
#include <QtWidgets\QLabel>
#include <QPalette>
#include "..\Header\ServerManager.hpp"
#include "ServerEntryContainer.hpp"

class ClientList : public QScrollArea
{
	Q_OBJECT
protected:
	static inline NosLib::DynamicArray<ServerEntryContainer*> serverEntryWidgetArray;

	QVBoxLayout* ChatFeedLayout;
	QWidget* ChatFeedWidget;

public slots:
	void AddNewServerEntry(ServerManager* newServerEntry)
	{
		AddServerEntry(newServerEntry);
	}

	void RemoveServerEntry(ServerManager* serverEntry)
	{
		for (int i = 0; i <= serverEntryWidgetArray.GetLastArrayIndex(); i++)
		{
			if (serverEntry != serverEntryWidgetArray[i]->Server)
			{
				continue;
			}

			this->widget()->layout()->removeWidget(serverEntryWidgetArray[i]);
			serverEntryWidgetArray.Remove(i);
			break;
		}
		this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
		QCoreApplication::processEvents();
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

	void AddServerEntry(ServerManager* newServerEntry)
	{
		ServerEntryContainer* newServerEntryContainer = new ServerEntryContainer(newServerEntry, this);
		serverEntryWidgetArray.Append(newServerEntryContainer);
		this->widget()->layout()->addWidget(newServerEntryContainer);

		this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
		QCoreApplication::processEvents();
	}

	template <typename ConnectFunc>
	void AddServerEntry(ServerManager* newServerEntry, ConnectFunc&& slot)
	{
		ServerEntryContainer* newServerEntryContainer = new ServerEntryContainer(newServerEntry, this);
		connect(newServerEntryContainer, &ServerEntryContainer::MouseReleased, slot);

		serverEntryWidgetArray.Append(newServerEntryContainer);
		this->widget()->layout()->addWidget(newServerEntryContainer);

		this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
		QCoreApplication::processEvents();
	}
};