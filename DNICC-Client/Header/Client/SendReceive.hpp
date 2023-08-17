#pragma once
#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

#include <QThread>

#include <Central/Misc.hpp>
#include <Central/Logging.hpp>

#include "..\Communication.hpp"
#include "..\GlobalRoot\GlobalRoot_Int.hpp"
#include "..\GlobalRoot\GlobalRoot_Ext.hpp"

namespace SendReceive
{
	class ChatListenThread : public QThread
	{
		Q_OBJECT

	signals:
		void ReceivedMessage(Communications::MessageObject receivedMessage);

	protected:
		void run() override
		{
			boost::system::error_code errorCode;

			while (!isInterruptionRequested() && errorCode != boost::asio::error::eof)
			{
				boost::asio::streambuf MessageBuffer;
				boost::asio::read_until((*GlobalRoot::ConnectionSocket), MessageBuffer, Definition::Delimiter, errorCode);

				Communications::MessageObject messageObject(&MessageBuffer); /* Create message object */

				emit ReceivedMessage(messageObject);
			}

			if (errorCode == boost::asio::error::eof)
			{
				Central::Logging::CreateLog<wchar_t>(L"listen thread quit, reason: end of file\n", false);
			}

			/* send some closing message */
		}
	};

	class ChatSend : public QObject
	{
		Q_OBJECT

	signals:
		void HostSendMessage(std::wstring message);

	protected:
		ChatSend(){}
	public:
		static ChatSend& GetInstance()
		{
			static ChatSend instance;
			return instance;
		}

		static inline void SendMessage(const std::wstring& message)
		{
			/* if is host, send message directly to self using signals */
			if (GlobalRoot::ThisClient->GetClientType() == SelfClient::enClientType::Host)
			{
				emit GetInstance().HostSendMessage(message);
				return;
			}
			/* else, just send message normally */

			std::string temp = NosLib::String::ToString(message);
			Central::Write(GlobalRoot::ConnectionSocket, boost::asio::buffer(temp));
		}
	};
}