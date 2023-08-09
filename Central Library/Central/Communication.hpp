#pragma once

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>

namespace CentralLib
{
    namespace Communications
    {
        class CentralizedServerResponse /* Centralized as in only including things that the client would need to understand the response */
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
            CentralizedServerResponse() {}

            CentralizedServerResponse(boost::asio::streambuf* Streambuf)
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

        class CentralizedClientResponse
        {
        public:
            enum class InformationCodes : uint8_t
            {
                GoingClientPath = 0,
                GoingHostingPath = 1,
                Ready = 2, /* Message from Client which tells the server that it is ready for the next step */
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
            CentralizedClientResponse() {}

            CentralizedClientResponse(boost::asio::streambuf* Streambuf)
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
}