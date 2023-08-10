#ifndef _CLIENT_LIBRARY_NOSNET_HPP_
#define _CLIENT_LIBRARY_NOSNET_HPP_

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <string>

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStackedWidget>
#include "ui_MainWindow.h"

#include "Central\CentralLib.hpp"

namespace ClientLib
{
    /// <summary>
    /// namespace which will contains items which are to do with starting up
    /// </summary>
    namespace StartUp
    {
		enum class UserMode : uint8_t
		{
			Client = 0,
			Hosting = 1,
		};

		inline UserMode GatherClientMode(Ui::MainWindowClass* ui)
		{
			if (ui->NormalSelection->isChecked()) /* Normal client */
			{
				return UserMode::Client;
			}
			
			if (ui->HostSelection->isChecked()) /* Host */
			{
				return UserMode::Hosting;
			}

			return UserMode::Client;
		}
    }
}
#endif