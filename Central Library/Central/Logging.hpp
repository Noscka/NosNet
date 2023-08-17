#pragma once
#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <NosLib/DynamicArray.hpp>
#include <NosLib/String.hpp>

#include <chrono>

namespace Central
{
	class Logging
	{
	protected:
		static inline NosLib::DynamicArray<Logging*> Logs;

		std::wstring LogMessage;
		std::chrono::system_clock::time_point LogTimestamp;

		Logging() {}

		Logging(const std::wstring& logMessage)
		{
			LogMessage = logMessage;
			LogTimestamp = std::chrono::system_clock::now();
		}

	public:
		template<typename CharType>
		static inline Logging* CreateLog(const std::basic_string<CharType>& strIn, const bool& printLog = false)
		{
			Logging* logObject = new Logging(NosLib::String::ConvertString<wchar_t, CharType>(strIn));
			Logs.Append(logObject);

			std::wstring containedLogMessage = logObject->GetLog();
			if (printLog)
			{
				(void)wprintf(containedLogMessage.c_str());
			}

			std::wofstream outLog(L"log.txt", std::ios::binary | std::ios::app);
			outLog.write(containedLogMessage.c_str(), containedLogMessage.size());
			outLog.close();

			return logObject;
		}

		std::wstring GetLog()
		{
			// %d/%m/%Y for date too
			return std::format(L"{:%X}  {}", std::chrono::zoned_time(std::chrono::current_zone(), LogTimestamp), LogMessage);
		}
	};
}