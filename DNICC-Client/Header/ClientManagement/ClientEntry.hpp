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
		}
		template<class Archive>
		void load(Archive& archive, const unsigned int version)
		{
			archive& ClientName;
			archive& ClientStatus;
		}
		BOOST_SERIALIZATION_SPLIT_MEMBER()

		std::wstring ClientName; /* Client's name */
		enClientStatus ClientStatus; /* Client's status, online or offline */

		/// <summary>
		/// Constructor
		/// </summary>
		ClientEntry(const std::wstring& clientName, const enClientStatus& clientStatus)
		{
			ClientName = clientName;
			ClientStatus = clientStatus;

			ClientRegistry.Append(this);
		}

	public:
		ClientEntry() {}

		~ClientEntry() /* on destruction, delete self from Array */
		{
			ClientRegistry.ObjectRemove(this, false, false);
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