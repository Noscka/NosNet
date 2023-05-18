#ifndef _SERVER_LIBRARY_NOSNET_HPP_
#define _SERVER_LIBRARY_NOSNET_HPP_

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>
#include <string.h>
#include <string>

#include <boost/asio.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>

#include <NosStdLib/DynamicArray.hpp>
#include <NosStdLib/String.hpp>

#include <Central/CentralLib.hpp>

namespace ServerLib
{
	namespace Communications
	{
		class ServerResponse : public CentralLib::Communications::CentralizedServerResponse
		{
		public:
			ServerResponse(const InformationCodes& informationCode, const std::wstring& additionalInformation)
			{
				InformationCode = informationCode;
				AdditionalInformation = additionalInformation;
			}
		};
	}
}
#endif