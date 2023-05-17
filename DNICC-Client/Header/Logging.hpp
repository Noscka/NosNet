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
			class MessageLog
			{
			private:
				std::wstring Message;
				std::chrono::system_clock::time_point LogTimestamp;

			public:
				MessageLog(){ }

				MessageLog(const std::wstring& message)
				{
					Message = message;
					LogTimestamp = std::chrono::system_clock::now();
				}

				std::wstring ConvertToLog()
				{
					// %d/%m/%Y for date too

					return std::format(L"{:%X}  {}", std::chrono::zoned_time(std::chrono::current_zone(), LogTimestamp), Message);
				}
			};

			NosStdLib::DynamicArray<MessageLog*> LoggedMessages;
		}

		template<typename CharType>
		void LogMessage(const std::basic_string<CharType>& strIn)
		{
			MessageLog* messageLogObject = new MessageLog(NosStdLib::String::ConvertString<wchar_t, CharType>(strIn));
			LoggedMessages.Append(messageLogObject);
			wprintf(messageLogObject->ConvertToLog().c_str());
			return;
		}
	}
}

#endif