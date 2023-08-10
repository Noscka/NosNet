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

#include <NosLib/String.hpp>
#include <NosLib/Cast.hpp>

#include <string.h>
#include <format>
#include <iostream>
#include <fstream>

namespace Definition
{
	const std::string Delimiter = "\n\r\n\r\n\013\x4\n";
	typedef unsigned char byte;
}

namespace Constants
{
    /* Default Connection info */
    inline const std::string DefaultPort = "58233";
    inline const std::string DefaultHostname = "localhost";

    /* Connection info for update service */
    inline const std::string UpdateServiceHostName = DefaultHostname;
    inline const std::string UpdateServicePort = DefaultPort;

    /* Default Client host info */
    inline const std::string DefaultClientHostPort = "58234";
}

namespace CentralLib
{
	inline std::wstring ReturnAddress(const boost::asio::ip::tcp::endpoint& Endpoint)
	{
		return std::format(L"{}:{}", NosLib::String::ConvertString<wchar_t, char>(Endpoint.address().to_v4().to_string()), NosLib::String::ConvertString<wchar_t, char>(std::to_string(Endpoint.port())));
	}

    /// <summary>
    /// convert stream buffer to wide string and removing delimiter
    /// </summary>
    /// <param name="streamBuffer"> - stream buffer pointer needed </param>
    /// <param name="bytes_received"> - amount of bytes received</param>
    /// <returns>wide string</returns>
    inline std::wstring streamBufferToWstring(boost::asio::streambuf* streamBuffer, const size_t& bytes_received)
    {
        return std::wstring{boost::asio::buffers_begin(streamBuffer->data()), boost::asio::buffers_begin(streamBuffer->data()) + bytes_received - Definition::Delimiter.size()};
    }

    inline size_t Write(boost::asio::ip::tcp::socket* connectedSocket, boost::asio::mutable_buffers_1 b)
    {
		size_t firstWrite = boost::asio::write((*connectedSocket), b);
		size_t secondWrite = boost::asio::write((*connectedSocket), boost::asio::buffer(Definition::Delimiter));

		return firstWrite + secondWrite;
    }

    template<typename Allocator>
	inline size_t Write(boost::asio::ip::tcp::socket* connectedSocket, boost::asio::basic_streambuf<Allocator>& b)
    {
		size_t firstWrite = boost::asio::write((*connectedSocket), boost::asio::basic_streambuf_ref<Allocator>(b));
		size_t secondWrite = boost::asio::write((*connectedSocket), boost::asio::buffer(Definition::Delimiter));

        return firstWrite + secondWrite;
    }

    namespace ClientInterfacing
    {
        class StrippedClientTracker : public NosLib::ArrayPositionTrack::PositionTrack
        {
        public:
            /// <summary>
            /// Emun which is used to define the clients last known status
            /// </summary>
            enum class UserStatus : uint8_t
            {
                /* User Type */
                Client = 0b10000000,
                Hosting = 0b01000000,

                /* User Status */
                Online = 0b00000001,
                Offline = 0b00000010,
            };

        private:
            friend boost::serialization::access;

            template<class Archive>
            void save(Archive& archive, const unsigned int version) const
            {
				archive& ClientUsername;
				archive& ClientCurrentStatus;
				archive& TargetEndpoint->address().to_string();
				archive& NosLib::Cast<int>(TargetEndpoint->port());
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

                TargetEndpoint = new boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(ipAddress), port);
            }
            BOOST_SERIALIZATION_SPLIT_MEMBER()

        protected:
            static inline NosLib::DynamicArray<StrippedClientTracker*> ClientArray; /* Array of all clients that have joined */

            std::wstring ClientUsername; /* Client's name which they choose */
            UserStatus ClientCurrentStatus; /* Client's status on if they are online, offline and etc */
            boost::asio::ip::tcp::endpoint* TargetEndpoint; /* Session's ConnectionSocket to get the endpoint from */

			/// <summary>
			/// Constructor
			/// </summary>
            StrippedClientTracker(const std::wstring& clientUsername, const UserStatus& clientCurrentStatus, boost::asio::ip::tcp::endpoint* targetEndpoint)
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
            /// Get the current client's ip address as a string
            /// </summary>
            /// <returns>the ip address</returns>
            std::string ReturnIPAddress()
            {
                return TargetEndpoint->address().to_v4().to_string();
            }

			/// <summary>
			/// Get the current client's username
			/// </summary>
			/// <returns>username</returns>
            std::wstring GetUsername()
            {
                return ClientUsername;
            }

            std::wstring GetUserStatusAsWstring()
            {
                switch (ClientCurrentStatus)
                {
                case CentralLib::ClientInterfacing::StrippedClientTracker::UserStatus::Client:
					return L"Client";
                case CentralLib::ClientInterfacing::StrippedClientTracker::UserStatus::Hosting:
                    return L"Hosting";
                case CentralLib::ClientInterfacing::StrippedClientTracker::UserStatus::Online:
                    return L"Online";
                case CentralLib::ClientInterfacing::StrippedClientTracker::UserStatus::Offline:
                    return L"Offline";
                }

                return L"Unknown";
            }

			/// <summary>
			/// (FOR TESTING) just used to list all the ips that are in the array
			/// </summary>
			/// <returns></returns>
			static std::wstring ListClientArray()
			{
                std::wstring output;
                for (int i = 0; i <= ClientArray.GetLastArrayIndex(); i++)
                {
                    output += std::format(L"{}) {} : {}\n",i, ClientArray[i]->ClientUsername, CentralLib::ReturnAddress(*(ClientArray[i]->TargetEndpoint)));
                }
                return output;
			}

            static NosLib::DynamicArray<StrippedClientTracker*>* GetClientArray()
            {
                return &ClientArray;
            }

            /// <summary>
            /// Function used to serialize the whole array into streambuf
            /// </summary>
            /// <param name="Streambuf">- pointer to streambuf that will be serialized</param>
            /// <param name="positionToIngore">(default = -1) - position of position to ignore when serializing</param>
            static void SerializeArray(std::streambuf* Streambuf, const int& positionToIngore = -1)
            {
                boost::archive::binary_oarchive oa(*Streambuf);
                int clientHostingCount = 0;
                for (int i = 0; i <= ClientArray.GetLastArrayIndex(); i++) /* Count amount of Client Servers in array */
                {
                    if (ClientArray[i]->ClientCurrentStatus == UserStatus::Hosting)
                    {
                        clientHostingCount++;
                    }
                }

                /* Serialize the int containing the amount of entries in array */
                oa& (clientHostingCount - (positionToIngore != -1 ? (ClientArray[positionToIngore]->ClientCurrentStatus == UserStatus::Hosting ? 1 : 0) : 0));

                for (int i = 0; i <= ClientArray.GetLastArrayIndex(); i++)
                {
                    if (positionToIngore == i) /* if the index is the same as positionToIgnore, then ignore it */
                    {
                        continue;
                    }

                    if (ClientArray[i]->ClientCurrentStatus != UserStatus::Hosting) /* if user isn't a client server, ignore */
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

    namespace ClientManagement
    {
        class ClientTracker : public CentralLib::ClientInterfacing::StrippedClientTracker
        {
        private:
            friend boost::serialization::access;
            template<class Archive>
            void serialize(Archive& archive, const unsigned int version)
            {
                // serialize base class information
                archive& boost::serialization::base_object<CentralLib::ClientInterfacing::StrippedClientTracker>(*this);
            }

            //static inline NosLib::DynamicArray<ClientTracker*> ClientArray; /* Array of all clients that have joined */

            boost::asio::ip::tcp::socket* SessionConnectionSocket; /* Session's ConnectionSocket to get the endpoint from */
       
            /// <summary>
            /// Constructor
            /// </summary>
            ClientTracker(const std::wstring& clientUsername, const UserStatus& clientCurrentStatus, boost::asio::ip::tcp::socket* sessionConnectionSocket)
            {
                ClientUsername = clientUsername;
                ClientCurrentStatus = clientCurrentStatus;
                SessionConnectionSocket = sessionConnectionSocket;

                boost::asio::ip::tcp::endpoint* tempPointer = new boost::asio::ip::tcp::endpoint(sessionConnectionSocket->remote_endpoint());
                TargetEndpoint = tempPointer;

                ClientArray.Append(this);
            }

        public:

            boost::asio::ip::tcp::socket* GetConnectionSocket()
            {
                return SessionConnectionSocket;
            }

            /// <summary>
            /// Changes clients status
            /// </summary>
            void ChangeStatus(const UserStatus& newClientStatus)
            {
                ClientCurrentStatus = newClientStatus;
            }

            /// <summary>
            /// Static contructor as the created object *NEED* to be created with the "new" keyword to stay alive
            /// </summary>
            /// <param name="clientUsername">- The client defined username</param>
            /// <param name="clientCurrentStatus">- the clients status when created</param>
            /// <param name="sessionConnectionSocket">- pointer to the clients endpoint (remote_endpoint)</param>
            /// <returns>pointer to self</returns>
            static ClientTracker* RegisterClient(const std::wstring& clientUsername, const UserStatus& clientCurrentStatus, boost::asio::ip::tcp::socket* sessionConnectionSocket)
            {
                return new ClientTracker(clientUsername, clientCurrentStatus, sessionConnectionSocket);
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
        inline bool ValidateUsername(const std::wstring& username)
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