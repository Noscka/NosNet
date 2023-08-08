#ifndef _CLIENT_SENDRECEIVE_NOSNET_
#define _CLIENT_SENDRECEIVE_NOSNET_

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

#include <QThread>

#include <Central/CentralLib.hpp>
#include <Central/Logging.hpp>

#include "..\Communication.hpp"
#include "..\GlobalRoot.hpp"

namespace ClientLib
{
	namespace Client
	{
		class ChatListenThread : public QThread
		{
			Q_OBJECT

		signals:
			void ReceivedMessage(ClientLib::Communications::MessageObject receivedMessage);

		protected:
			void run() override
			{
				boost::system::error_code errorCode;

				while (!isInterruptionRequested() && errorCode != boost::asio::error::eof)
				{
					boost::asio::streambuf MessageBuffer;
					boost::asio::read_until((*GlobalRoot::ConnectionSocket), MessageBuffer, Definition::Delimiter, errorCode);

					ClientLib::Communications::MessageObject messageObject(&MessageBuffer); /* Create message object */

					emit ReceivedMessage(messageObject);
				}

				if (errorCode == boost::asio::error::eof)
				{
					CentralLib::Logging::CreateLog<wchar_t>(L"listen thread quit, reason: end of file\n", false);
				}

				/* send some closing message */
			}
		};

		inline void SendMessage(const std::wstring& message)
		{
			std::string temp = NosLib::String::ToString(message);
			CentralLib::Write(GlobalRoot::ConnectionSocket, boost::asio::buffer(temp));
		}
	}
}
#endif /* _CLIENT_SENDRECEIVE_NOSNET_ */