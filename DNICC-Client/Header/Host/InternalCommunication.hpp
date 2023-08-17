#pragma once
#include "..\Communication.hpp"

namespace InternalCommunication
{
	class HostResponse : public InternalHostResponse
	{
	public:
		HostResponse(const InformationCodes& informationCode, const std::wstring& additionalInformation)
		{
			InformationCode = informationCode;
			AdditionalInformation = additionalInformation;
		}

		static void CreateSerializeSend(boost::asio::ip::tcp::socket* connectionSocket, const InformationCodes& informationCode, const std::wstring& additionalInformation)
		{
			boost::asio::streambuf tempBuf;
			Central::Write(connectionSocket, *(HostResponse(informationCode, additionalInformation).SerializeObject(&tempBuf)));
		}
	};
}