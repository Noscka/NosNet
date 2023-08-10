#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

#include "Header/Listen.hpp"
#include "Header/GlobalRoot.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindowClass; };
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr) : QMainWindow(parent)
	{
		ui = new Ui::MainWindowClass();
		GlobalRoot::UI = ui;

		ui->setupUi(this);
		ServerLib::Listen::StartDCHLS();
	}

	~MainWindow()
	{
		delete ui;
	}

private:
    Ui::MainWindowClass* ui;
};
