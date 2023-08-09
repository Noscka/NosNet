#pragma once

#include <QtWidgets/QApplication>
#include "ui_MainWindow.h"

namespace GlobalRoot
{
	inline QApplication* AppPointer = nullptr;
	inline Ui::MainWindowClass* UI;
}