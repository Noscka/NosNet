#pragma once

#include <QtWidgets/QWidget.h>
#include <QtWidgets/QLayout.h>

class CenterLayout : public QWidget
{
	Q_OBJECT

public:
	CenterLayout(QWidget* parent = nullptr)
	{
		QList<QWidget*> childrenList = findChildren<QWidget*>(Qt::FindChildOption::FindDirectChildrenOnly);

		childrenList.length();

		for (QWidget* entry : childrenList)
		{

		}
	}
};