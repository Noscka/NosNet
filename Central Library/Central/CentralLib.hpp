#ifndef _CENTRAL_LIBRARY_NOSNET_HPP_
#define _CENTRAL_LIBRARY_NOSNET_HPP_

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>

#include <NosStdLib/String.hpp>
#include <NosStdLib/Cast.hpp>

#include <string.h>
#include <format>
#include <iostream>
#include <fstream>

namespace Definition
{
	const std::string Delimiter = "\n\r\n\r\n\013\x4\n";
	typedef unsigned char byte;
}

namespace CentralLib
{
	std::wstring ReturnAddress(const boost::asio::ip::tcp::endpoint& Endpoint)
	{
		return std::format(L"{}:{}", NosStdLib::String::ConvertString<wchar_t, char>(Endpoint.address().to_v4().to_string()), NosStdLib::String::ConvertString<wchar_t, char>(std::to_string(Endpoint.port())));
	}

    /// <summary>
    /// convert stream buffer to wide string and removing delimiter
    /// </summary>
    /// <param name="streamBuffer"> - stream buffer pointer needed </param>
    /// <param name="bytes_received"> - amount of bytes received</param>
    /// <returns>wide string</returns>
    std::wstring streamBufferToWstring(boost::asio::streambuf* streamBuffer, const size_t& bytes_received)
    {
        return std::wstring{boost::asio::buffers_begin(streamBuffer->data()), boost::asio::buffers_begin(streamBuffer->data()) + bytes_received - Definition::Delimiter.size()};
    }

    namespace ClientInterfacing
    {
        class StrippedClientTracker : public NosStdLib::ArrayPositionTrack::PositionTrack
        {
        public:
            /// <summary>
            /// Emun which is used to define the clients last known status
            /// </summary>
            enum class ClientStatus : uint8_t
            {
                Offline = 0,
                Online = 1,
            };

        private:
            friend boost::serialization::access;

            template<class Archive>
            void save(Archive& archive, const unsigned int version) const
            {
				archive& ClientUsername;
				archive& ClientCurrentStatus;
				archive& TargetEndpoint->address().to_string();
				archive& NosStdLib::Cast::Cast<int>(TargetEndpoint->port());
            }
            template<class Archive>
            void load(Archive& archive, const unsigned int version)
            {
				archive& ClientUsername;
				archive& ClientCurrentStatus;

                std::string ipAddress;
                int port;

                archive& ipAddress;
                archive& port;

                TargetEndpoint = new boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(ipAddress), port);// , port);
            }
            BOOST_SERIALIZATION_SPLIT_MEMBER()

        protected:
            static inline NosStdLib::DynamicArray<StrippedClientTracker*> ClientArray; /* Array of all clients that have joined */

            std::wstring ClientUsername; /* Client's name which they choose */
            ClientStatus ClientCurrentStatus; /* Client's status on if they are online, offline and etc */
            boost::asio::ip::tcp::endpoint* TargetEndpoint; /* Session's ConnectionSocket to get the endpoint from */

			/// <summary>
			/// Constructor
			/// </summary>
            StrippedClientTracker(const std::wstring& clientUsername, const ClientStatus& clientCurrentStatus, boost::asio::ip::tcp::endpoint* targetEndpoint)
			{
				ClientUsername = clientUsername;
				ClientCurrentStatus = clientCurrentStatus;
                TargetEndpoint = targetEndpoint;

				ClientArray.Append(this);
			}

        public:
            StrippedClientTracker(){}

            ~StrippedClientTracker() /* on destruction, delete self from Array */
            {
                ClientArray.ObjectRemove(this, false, false);
                delete TargetEndpoint;
            }

			/// <summary>
			/// (FOR TESTING) just used to list all the ips that are in the array
			/// </summary>
			/// <returns></returns>
			static std::wstring ListClientArray()
			{
                std::wstring output;
                for (StrippedClientTracker* entry : ClientArray)
                {
                    output += std::format(L"{} : {}\n", entry->ClientUsername, CentralLib::ReturnAddress(*(entry->TargetEndpoint)));
                }
                return output;
			}

            /// <summary>
            /// Function used to serialize the whole array into streambuf
            /// </summary>
            /// <param name="Streambuf">- pointer to streambuf that will be serialized</param>
            /// <param name="positionToIngore">(default = -1) - position of position to ignore when serializing</param>
            static void SerializeArray(std::streambuf* Streambuf, const int& positionToIngore = -1)
            {
                boost::archive::binary_oarchive oa(*Streambuf);
                oa& (ClientArray.GetArrayIndexPointer() - (positionToIngore != -1 ? 1 : 0));

                for (int i = 0; i < ClientArray.GetArrayIndexPointer(); i++)
                {
                    if (positionToIngore == i) /* if the index is the same as positionToIgnore, then ignore it */
                    {
                        continue;
                    }

                    oa& ClientArray[i];
                }
            }

            /// <summary>
            /// Function used to deserialize streambuf into an array
            /// </summary>
            /// <param name="Streambuf">- pointer to streambuf containing the data</param>
            static void DeserializeArray(boost::asio::streambuf* Streambuf)
            {
				boost::archive::binary_iarchive ia(*Streambuf);
				int arraySize;
				ia& arraySize;

				for (int i = 0; i < arraySize; i++)
				{
					ClientArray.Append(new StrippedClientTracker());
					ia& ClientArray[i];
				}
            }
        };
    }

    namespace Communications
    {
        class CentralizedServerResponse /* Centralized as in only including things that the client would need to understand the response */
        {
        public:
            enum class InformationCodes : uint8_t
            {
                Successful = 0,
                Accepted = 1,
                NotAccepted = 2,
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
            CentralizedServerResponse(){}

            InformationCodes GetInformationCode()
            {
                return InformationCode;
            }

            std::wstring GetAdditionalInformation()
            {
                return AdditionalInformation;
            }

			void serializeObject(std::streambuf* Streambuf)
			{
				boost::archive::binary_oarchive oa(*Streambuf);
				oa& *this;
			}

			void DeserializeObject(boost::asio::streambuf* Streambuf)
			{
				boost::archive::binary_iarchive ia(*Streambuf);
				ia& *this;
			}
        };
    }

    namespace Validation
    {
        /// <summary>
        /// Validates usernames (central so changes are global)
        /// </summary>
        /// <param name="username">- username to validate</param>
        /// <returns>true if valid, False if invalid</returns>
        bool ValidateUsername(const std::wstring& username)
        {
            /*
			Current requirements:
				- Not empty
				- Not longer then 30 characters
			*/
            if (username.empty() || username.length() >= 30)
            {
                return false;
            }

            return true;
        }
    }
}
#endif