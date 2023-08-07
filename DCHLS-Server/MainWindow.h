#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindowClass; };
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	void Page1()
	{
		ui->stackedWidget->setCurrentIndex(1);
	}

	void Page2()
	{
		ui->stackedWidget->setCurrentIndex(0);
	}

	MainWindow(QWidget* parent = nullptr) : QMainWindow(parent), ui(new Ui::MainWindowClass())
	{
		ui->setupUi(this);
		connect(ui->pushButton_P1, &QPushButton::released, this, &MainWindow::Page1);
		connect(ui->pushButton_P2, &QPushButton::released, this, &MainWindow::Page2);
	}

	~MainWindow()
	{
		delete ui;
	}

private:
    Ui::MainWindowClass* ui;
};
