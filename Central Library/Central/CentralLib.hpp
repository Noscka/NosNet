#ifndef _CENTRAL_LIBRARY_NOSNET_HPP_
#define _CENTRAL_LIBRARY_NOSNET_HPP_

#define WIN32_LEAN_AND_MEAN 
#include <sdkddkver.h>

#include <boost/asio.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>

#include <NosStdLib/String.hpp>

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
    namespace ClientInterfacing
    {
        class StrippedClientTracker
        {
        public:
            /// <summary>
            /// Emun which is used to define the clients last known status
            /// </summary>
            enum ClientStatus : UINT8
            {
                Offline = 0,
                Online = 1,
            };

        protected:
            friend boost::serialization::access;

            template<class Archive>
            void serialize(Archive& archive, const unsigned int version)
            {
                archive& ClientUsername;
                archive& ClientCurrentStatus;
                std::wstring ipString = NosStdLib::String::ConvertString<wchar_t, char>(SessionConnectionSocket->remote_endpoint().address().to_v4().to_string());

                archive& ipString; /* TODO: Figure out direct serialization */
            }

            static inline NosStdLib::DynamicArray<StrippedClientTracker*> ClientArray; /* Array of all clients that have joined */


            std::wstring ClientUsername; /* Client's name which they choose */
            ClientStatus ClientCurrentStatus; /* Client's status on if they are online, offline and etc */
            boost::asio::ip::tcp::socket* SessionConnectionSocket; /* Session's ConnectionSocket to get the endpoint from */

        public:
            void serializeArray(std::streambuf* Streambuf)
            {
                boost::archive::binary_oarchive oa(*Streambuf);
                oa& ClientArray.GetArrayIndexPointer();
                for (int i = 0; i < ClientArray.GetArrayIndexPointer() - 1; i++)
                {
                    oa&* ClientArray[i];
                }
            }

            void DeserializeArray(boost::asio::streambuf* Streambuf)
            {
                boost::archive::binary_iarchive ia(*Streambuf);
                int arraySize;
                ia& arraySize;

                ia& boost::serialization::make_array<StrippedClientTracker*>(ClientArray.GetArray(), arraySize);

                for (int i = 0; i < arraySize - 1; i++)
                {
                    StrippedClientTracker* clientAdding = new StrippedClientTracker();

                    ia&* clientAdding;

                    ClientArray.Append(clientAdding);
                }
            }
        };
    }

    std::wstring ReturnAddress(boost::asio::ip::tcp::endpoint Endpoint)
    {
        return std::format(L"{}:{}", NosStdLib::String::ConvertString<wchar_t, char>(Endpoint.address().to_v4().to_string()), NosStdLib::String::ConvertString<wchar_t, char>(std::to_string(Endpoint.port())));
    }
}
#endif