#pragma once
#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <Central/Misc.hpp>
#include <Central/Communication.hpp>

#include "ClientManagement\ClientEntry.hpp"

namespace Communications
{
	class MessageObject
	{
	protected:
		ClientEntry* UserInfo;
		std::wstring Message;

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& archive, const unsigned int version)
		{
			archive& UserInfo;
			archive& Message;
		}
	public:
		MessageObject(ClientEntry* userInfo, const std::wstring& message)
		{
			UserInfo = userInfo;
			Message = message;
		}

		MessageObject(boost::asio::streambuf* Streambuf)
		{
			DeserializeObject(Streambuf);
		}

		ClientEntry* GetUserInfo()
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

		static void CreateSerializeSend(boost::asio::ip::tcp::socket* connectionSocket, ClientEntry* userInfo, const std::wstring& message)
		{
			boost::asio::streambuf tempBuf;
			Central::Write(connectionSocket, *(MessageObject(userInfo, message).SerializeObject(&tempBuf)));
		}
	};

	class ClientResponse : public Central::Communications::CentralizedClientResponse
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
			Central::Write(connectionSocket, *(ClientResponse(informationCode, additionalInformation).SerializeObject(&tempBuf)));
		}
	};
}

namespace InternalCommunication
{
	class InternalHostResponse /* Centralized as in only including things that the client would need to understand the response */
	{
	public:
		enum class InformationCodes : uint8_t
		{
			Successful = 0, /* Operation was successful */
			Accepted = 1, /* Input was accepted */
			NotAccepted = 2, /* Input was not accepted */
			Ready = 3, /* Message from Server which tells the Client that it is ready for the next step */
		};
	protected:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& archive, const unsigned int version)
		{
			archive& InformationCode;
			archive& AdditionalInformation;
		}

		InformationCodes InformationCode;
		std::wstring AdditionalInformation;
	public:
		InternalHostResponse() {}

		InternalHostResponse(boost::asio::streambuf* Streambuf)
		{
			DeserializeObject(Streambuf);
		}

		InformationCodes GetInformationCode()
		{
			return InformationCode;
		}

		std::wstring GetAdditionalInformation()
		{
			return AdditionalInformation;
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
	};
}