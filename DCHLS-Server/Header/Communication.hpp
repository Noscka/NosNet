#pragma once
#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>
#include <string>

#include <boost/asio.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>

#include <NosLib/DynamicArray.hpp>
#include <NosLib/String.hpp>

#include <Central/Communication.hpp>

namespace Communications
{
	class ServerResponse : public Central::Communications::CentralizedServerResponse
	{
	public:
		ServerResponse(const InformationCodes& informationCode, const std::wstring& additionalInformation)
		{
			InformationCode = informationCode;
			AdditionalInformation = additionalInformation;
		}

		static void CreateSerializeSend(boost::asio::ip::tcp::socket* connectionSocket, const InformationCodes& informationCode, const std::wstring& additionalInformation)
		{
			boost::asio::streambuf tempBuf;
			Central::Write(connectionSocket, *(ServerResponse(informationCode, additionalInformation).SerializeObject(&tempBuf)));
		}
	};
}