#pragma once

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
#include <NosLib/DynamicArray.hpp>

#include <string.h>
#include <format>
#include <iostream>
#include <fstream>

namespace ClientLib
{
	class ClientEntry : public NosLib::ArrayPositionTrack::PositionTrack
	{
	public:
		static inline NosLib::DynamicArray<ClientEntry*> ClientRegistry; /* Array of all clients connected */

		enum class enClientStatus : uint8_t
		{
			Offline = 0,
			Online = 1,
		};

	protected:
		friend boost::serialization::access;

		template<class Archive>
		void save(Archive& archive, const unsigned int version) const
		{
			archive& ClientName;
			archive& ClientStatus;
			archive& TargetEndpoint->address().to_string();
			archive& NosLib::Cast<int>(TargetEndpoint->port());
		}
		template<class Archive>
		void load(Archive& archive, const unsigned int version)
		{
			archive& ClientName;
			archive& ClientStatus;

			std::string ipAddress;
			int port;

			archive& ipAddress;
			archive& port;

			TargetEndpoint = new boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(ipAddress), port);
		}
		BOOST_SERIALIZATION_SPLIT_MEMBER()

		std::wstring ClientName; /* Client's name */
		enClientStatus ClientStatus; /* Client's status, online or offline */
		boost::asio::ip::tcp::endpoint* TargetEndpoint; /* Session's ConnectionSocket to get the endpoint from */

		/// <summary>
		/// Constructor
		/// </summary>
		ClientEntry(const std::wstring& clientName, const enClientStatus& clientStatus, boost::asio::ip::tcp::endpoint* targetEndpoint)
		{
			ClientName = clientName;
			ClientStatus = clientStatus;
			TargetEndpoint = targetEndpoint;

			ClientRegistry.Append(this);
		}

	public:
		ClientEntry() {}

		~ClientEntry() /* on destruction, delete self from Array */
		{
			ClientRegistry.ObjectRemove(this, false, false);
			delete TargetEndpoint;
		}

		/// <summary>
		/// Get the current client's username
		/// </summary>
		/// <returns>username</returns>
		std::wstring GetClientName()
		{
			return ClientName;
		}

		enClientStatus GetClientStatus()
		{
			return ClientStatus;
		}

		boost::asio::ip::tcp::endpoint* GetEndpoint()
		{
			return TargetEndpoint;
		}

		/// <summary>
		/// Get the current client's ip address as a string
		/// </summary>
		/// <returns>the ip address</returns>
		std::string GetIpAddressAsString()
		{
			return TargetEndpoint->address().to_v4().to_string();
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

			/* Serialize the int containing the amount of entries in array */
			oa& (clientHostingCount - (positionToIngore != -1 ? 1 : 0));

			for (int i = 0; i <= ClientRegistry.GetLastArrayIndex(); i++)
			{
				if (positionToIngore == i) /* if the index is the same as positionToIgnore, then ignore it */
				{
					continue;
				}

				oa& ClientRegistry[i];
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
				ClientRegistry.Append(new ClientEntry());
				ia& ClientRegistry[i];
			}
		}
	};
}