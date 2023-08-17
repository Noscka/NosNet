#pragma once
#include "ui_MainWindow.h"

/* this root contains only the ui, this causes most dependency loops */

namespace GlobalRoot
{
	inline Ui::MainWindowClass* UI;
}