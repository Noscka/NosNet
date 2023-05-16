#ifndef _CLIENT_LOGGING_NOSNET_HPP_
#define _CLIENT_LOGGING_NOSNET_HPP_

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <NosStdLib/DynamicArray.hpp>
#include <NosStdLib/String.hpp>

#include <chrono>

namespace ClientLib
{
	namespace Logging
	{
		namespace
		{
			struct MessageLog
			{
				std::wstring Message;
				std::chrono::time_point<std::chrono::system_clock> LogTimestamp;

				MessageLog(const std::wstring& message, const std::chrono::time_point<std::chrono::system_clock>& logTimestamp)
				{
					Message = message;
					LogTimestamp = logTimestamp;
				}
			};

			NosStdLib::DynamicArray<MessageLog> LoggedMessages;
		}

		template<typename CharType>
		std::basic_string<CharType>& LogMessage(const std::basic_string<CharType>& strIn)
		{
			LoggedMessages.Append(MessageLog(strIn, std::chrono::system_clock::now));
			wprintf(strIn.c_str());
			return strIn;
		}
	}
}

#endif