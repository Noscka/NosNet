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
	class ArraySendingTest
	{
	private:
		std::wstring Name;
		float PersonalizedID;

		static inline NosStdLib::DynamicArray<ArraySendingTest*> Collection;
    protected:
		friend boost::serialization::access;

		template<class Archive>
		void serialize(Archive& archive, const unsigned int version)
		{
			archive& Name;
			archive& PersonalizedID;
		}

	public:
		ArraySendingTest()
		{

		}

		ArraySendingTest(std::wstring name)
		{
			Name = name;

			PersonalizedID = 0.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (100.0f - 0.0f)));

			Collection.Append(this);
		}

		static void ListArray()
		{
			for (ArraySendingTest* entry : Collection)
			{
				wprintf(L"ID: %f\n", entry->PersonalizedID);
			}
		}

		static void SerializeArray(std::streambuf* Streambuf)
		{
			boost::archive::binary_oarchive oa(*Streambuf);
			int arraySize = Collection.GetArrayIndexPointer();
			oa& arraySize;
			wprintf(L"array size: %d\n", arraySize);

			for (int i = 0; i < Collection.GetArrayIndexPointer(); i++)
			{
				wprintf(L"index: %d\n", i);
				oa& Collection[i];
			}
		}

		static void DeserializeArray(std::streambuf* Streambuf)
		{
			boost::archive::binary_iarchive ia(*Streambuf);
			int arraySize;
			ia& arraySize;
			wprintf(L"array size: %d\n", arraySize);

			for (int i = 0; i < arraySize; i++)
			{
				wprintf(L"index: %d\n", i);
				Collection.Append(new ArraySendingTest());
				ia& Collection[i];
			}
		}

	};

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

			/// <summary>
			/// Constructor
			/// </summary>
            StrippedClientTracker(const std::wstring& clientUsername, const ClientStatus& clientCurrentStatus, boost::asio::ip::tcp::socket* sessionConnectionSocket)
			{
				ClientUsername = clientUsername;
				ClientCurrentStatus = clientCurrentStatus;
				SessionConnectionSocket = sessionConnectionSocket;

				ClientArray.Append(this);
			}

        public:
            StrippedClientTracker()
            {

            }

			/// <summary>
			/// Static contructor as the created object *NEED* to be created with the "new" keyword to stay alive
			/// </summary>
			/// <param name="clientUsername">- The client defined username</param>
			/// <param name="clientCurrentStatus">- the clients status when created</param>
			/// <param name="sessionConnectionSocket">- pointer to the clients endpoint (remote_endpoint)</param>
			/// <returns>pointer to self</returns>
			static StrippedClientTracker* RegisterClient(const std::wstring& clientUsername, const ClientStatus& clientCurrentStatus, boost::asio::ip::tcp::socket* sessionConnectionSocket)
			{
				return new StrippedClientTracker(clientUsername, clientCurrentStatus, sessionConnectionSocket);
			}

            static void SerializeArray(std::streambuf* Streambuf)
            {
                boost::archive::binary_oarchive oa(*Streambuf);
                oa& ClientArray.GetArrayIndexPointer();
                for (int i = 0; i < ClientArray.GetArrayIndexPointer() - 1; i++)
                {
                    oa&* ClientArray[i];
                }
            }

            static void DeserializeArray(boost::asio::streambuf* Streambuf)
            {
                boost::archive::binary_iarchive ia(*Streambuf);
                int arraySize;
                ia& arraySize;

                ia& boost::serialization::make_array<StrippedClientTracker*>(ClientArray.GetArray(), arraySize);

				//for (int i = 0; i < arraySize - 1; i++)
				//{
				//    StrippedClientTracker* clientAdding = new StrippedClientTracker();
				//
				//    ia&* clientAdding;
				//
				//    ClientArray.Append(clientAdding);
				//}
            }
        };
    }

    std::wstring ReturnAddress(boost::asio::ip::tcp::endpoint Endpoint)
    {
        return std::format(L"{}:{}", NosStdLib::String::ConvertString<wchar_t, char>(Endpoint.address().to_v4().to_string()), NosStdLib::String::ConvertString<wchar_t, char>(std::to_string(Endpoint.port())));
    }
}
#endif