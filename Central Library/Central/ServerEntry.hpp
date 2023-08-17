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

namespace Central
{
	class ServerEntry : public NosLib::ArrayPositionTrack::PositionTrack
	{
		/* for when I make ServerType and ServerStatus combined with a "flag" system, 0b100 is binary */

	public:
		static inline NosLib::DynamicArray<ServerEntry*> ServerRegistry; /* Array of all servers */

		/// <summary>
		/// Emun which is used to define the clients last known status
		/// </summary>
		enum class enServerType : uint8_t
		{
			Direct = 0,		/* testing */
			Group = 1,
			Dedicated = 2,
		};

		enum class enServerStatus : uint8_t
		{
			Offline = 0,
			Online = 1,
		};

	private:
		friend boost::serialization::access;

		template<class Archive>
		void save(Archive& archive, const unsigned int version) const
		{
			archive& ServerName;
			archive& ServerType;
			archive& ServerStatus;
			archive& TargetEndpoint->address().to_string();
			archive& NosLib::Cast<int>(TargetEndpoint->port());
		}
		template<class Archive>
		void load(Archive& archive, const unsigned int version)
		{
			archive& ServerName;
			archive& ServerType;
			archive& ServerStatus;

			std::string ipAddress;
			int port;

			archive& ipAddress;
			archive& port;

			TargetEndpoint = new boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(ipAddress), port);
		}
		BOOST_SERIALIZATION_SPLIT_MEMBER()

	protected:
		std::wstring ServerName; /* Server's name */
		enServerType ServerType; /* what the server type is, Direct, Group or Dedicated */
		enServerStatus ServerStatus; /* Server's status, online or offline */
		boost::asio::ip::tcp::endpoint* TargetEndpoint; /* Session's ConnectionSocket to get the endpoint from */

		/// <summary>
		/// Constructor
		/// </summary>
		ServerEntry(const std::wstring& serverName, const enServerType& serverType, const enServerStatus& serverStatus, boost::asio::ip::tcp::endpoint* targetEndpoint)
		{
			ServerName = serverName;
			ServerType = serverType;
			ServerStatus = serverStatus;
			TargetEndpoint = targetEndpoint;

			ServerRegistry.Append(this);
		}

	public:
		ServerEntry() {}

		~ServerEntry() /* on destruction, delete self from Array */
		{
			ServerRegistry.ObjectRemove(this, false, false);
			delete TargetEndpoint;
		}

		/// <summary>
		/// Get the current client's username
		/// </summary>
		/// <returns>username</returns>
		std::wstring GetServerName()
		{
			return ServerName;
		}

		enServerType GetServerType()
		{
			return ServerType;
		}

		enServerStatus GetServerStatus()
		{
			return ServerStatus;
		}

		boost::asio::ip::tcp::endpoint* GetEndpoint()
		{
			return TargetEndpoint;
		}

		/// <summary>
		/// Get the current client's ip address as a string
		/// </summary>
		/// <returns>the ip address</returns>
		std::wstring GetIpAddressAsWString()
		{
			return NosLib::String::ToWstring(TargetEndpoint->address().to_v4().to_string());
		}

		/// <summary>
		/// Function used to serialize the whole array into streambuf
		/// </summary>
		/// <param name="Streambuf">- pointer to streambuf that will be serialized</param>
		/// <param name="positionToIngore">(default = -1) - position of position to ignore when serializing</param>
		static void SerializeArray(std::streambuf* Streambuf, const int& positionToIngore = -1)
		{
			boost::archive::binary_oarchive oa(*Streambuf);

			/* Serialize the int containing the amount of entries in array */
			oa& (ServerRegistry.GetItemCount() - (positionToIngore != -1 ? 1 : 0));

			for (int i = 0; i <= ServerRegistry.GetLastArrayIndex(); i++)
			{
				if (positionToIngore == i) /* if the index is the same as positionToIgnore, then ignore it */
				{
					continue;
				}

				oa& ServerRegistry[i];
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
				ServerRegistry.Append(new ServerEntry());
				ia& ServerRegistry[i];
			}
		}
	};
}