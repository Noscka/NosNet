#ifndef _CLIENT_COMMUNICATION_NOSNET_HPP_
#define _CLIENT_COMMUNICATION_NOSNET_HPP_

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <Central/CentralLib.hpp>
#include <Central/Communication.hpp>

namespace ClientLib
{
	namespace Communications
	{

		class MessageObject
		{
		protected:
			CentralLib::ClientInterfacing::StrippedClientTracker* UserInfo;
			std::wstring Message;

			friend class boost::serialization::access;
			template<class Archive>
			void serialize(Archive& archive, const unsigned int version)
			{
				archive& UserInfo;
				archive& Message;
			}
		public:
			MessageObject(CentralLib::ClientInterfacing::StrippedClientTracker* userInfo, const std::wstring& message)
			{
				UserInfo = userInfo;
				Message = message;
			}

			MessageObject(boost::asio::streambuf* Streambuf)
			{
				DeserializeObject(Streambuf);
			}

			CentralLib::ClientInterfacing::StrippedClientTracker* GetUserInfo()
			{
				return UserInfo;
			}

			std::wstring GetMessage()
			{
				return Message;
			}

			boost::asio::streambuf* SerializeObject(boost::asio::streambuf* Streambuf)
			{
				boost::archive::binary_oarchive oa(*Streambuf);
				oa&* this;
				return Streambuf;
			}

			void DeserializeObject(boost::asio::streambuf* Streambuf)
			{
				boost::archive::binary_iarchive ia(*Streambuf);
				ia&* this;
			}

			static void CreateSerializeSend(boost::asio::ip::tcp::socket* connectionSocket, CentralLib::ClientInterfacing::StrippedClientTracker* userInfo, const std::wstring& message)
			{
				boost::asio::streambuf tempBuf;
				CentralLib::Write(connectionSocket, *(MessageObject(userInfo, message).SerializeObject(&tempBuf)));
			}
		};

		class ClientResponse : public CentralLib::Communications::CentralizedClientResponse
		{
		public:
			ClientResponse(const InformationCodes& informationCode, const std::wstring& additionalInformation)
			{
				InformationCode = informationCode;
				AdditionalInformation = additionalInformation;
			}

			static void CreateSerializeSend(boost::asio::ip::tcp::socket* connectionSocket, const InformationCodes& informationCode, const std::wstring& additionalInformation)
			{
				boost::asio::streambuf tempBuf;
				CentralLib::Write(connectionSocket, *(ClientResponse(informationCode, additionalInformation).SerializeObject(&tempBuf)));
			}
		};
	}
}
#endif