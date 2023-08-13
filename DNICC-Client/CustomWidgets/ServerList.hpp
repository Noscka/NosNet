#pragma once
#include <QCoreApplication>
#include <QtWidgets\QLayout>
#include <QtWidgets\QScrollBar>
#include <QtWidgets/QScrollArea>
#include <QtWidgets\QLabel>
#include <QPalette>
#include <Central\ServerEntry.hpp>
#include "ServerEntryContainer.hpp"

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

	template <typename ConnectFunc>
	void AddServerEntry(CentralLib::ServerEntry* server, ConnectFunc&& slot)
	{
		ServerEntryContainer* newServerEntryContainer = new ServerEntryContainer(server, this);
		connect(newServerEntryContainer, &ServerEntryContainer::MouseReleased, slot);

		this->widget()->layout()->addWidget(newServerEntryContainer);

		this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
		QCoreApplication::processEvents();
	}
};