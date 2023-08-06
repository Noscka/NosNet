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
	MainWindow(QWidget* parent = nullptr) : QMainWindow(parent), ui(new Ui::MainWindowClass())
	{
		ui->setupUi(this);
	}

    ~MainWindow()
	{
		delete ui;
	}

private:
    Ui::MainWindowClass* ui;
};
