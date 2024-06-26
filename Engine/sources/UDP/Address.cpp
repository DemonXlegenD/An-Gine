#include "UDP/Address.h"
#include "UDP/Serialization/Convert.h"

#include <WS2tcpip.h>
#include <sstream>

namespace Bousk
{
	namespace Network
	{
		Address::Address(const Address& src) noexcept
			: mPort(src.mPort)
			, mType(src.mType)
		{
			memcpy(&mStorage, &(src.mStorage), sizeof(mStorage));
		}
		Address::Address(Address&& src) noexcept
			: mPort(src.mPort)
			, mType(src.mType)
		{
			memcpy(&mStorage, &(src.mStorage), sizeof(mStorage));
		}
		Address& Address::operator=(const Address& src) noexcept
		{
			mPort = src.mPort;
			mType = src.mType;
			memcpy(&mStorage, &(src.mStorage), sizeof(mStorage));
			return *this;
		}
		Address& Address::operator=(Address&& src) noexcept
		{
			mPort = src.mPort;
			mType = src.mType;
			memcpy(&mStorage, &(src.mStorage), sizeof(mStorage));
			return *this;
		}

		Address::Address(const std::string& ip, uint16 port) noexcept : mPort(port)
		{
			memset(&mStorage, 0, sizeof(mStorage));
			if (!ip.empty())
			{
				//IPv4
				{
					sockaddr_in& addrin = reinterpret_cast<sockaddr_in&>(mStorage);
					in_addr& inaddr = addrin.sin_addr;
					if (inet_pton(AF_INET, ip.c_str(), &inaddr) == 1)
					{
						mType = Type::IPv4;
						addrin.sin_family = AF_INET;
						Serialization::Conversion::ToNetwork(mPort, addrin.sin_port);
						return;
					}
				}
				{
					sockaddr_in6& addrin = reinterpret_cast<sockaddr_in6&>(mStorage);
					in_addr6& inaddr = addrin.sin6_addr;
					if (inet_pton(AF_INET6, ip.c_str(), &inaddr) == 1)
					{
						mType = Type::IPv6;
						addrin.sin6_family = AF_INET6;
						Serialization::Conversion::ToNetwork(mPort, addrin.sin6_port);
						return;
					}
				}
			}
		}

		Address::Address(const sockaddr_storage& addr) noexcept
		{
			set(addr);
		}

		// permet de d�terminet si c'est une adresse IPv4 ou IPv6
		void Address::set(const sockaddr_storage& src)
		{
			memcpy(&mStorage, &src, sizeof(mStorage));
			if (mStorage.ss_family == AF_INET)
			{
				mType = Type::IPv4;
				const sockaddr_in& addrin = reinterpret_cast<const sockaddr_in&>(mStorage);
				Serialization::Conversion::ToLocal(addrin.sin_port, mPort);
			}
			else if (mStorage.ss_family == AF_INET6)
			{
				mType = Type::IPv6;
				const sockaddr_in6& addrin = reinterpret_cast<const sockaddr_in6&>(mStorage);
				Serialization::Conversion::ToLocal(addrin.sin6_port, mPort);
			}
			else
			{
				mType = Type::None;
			}
		}

		// Permet de prendre en charge tout type d'adresses IP
		Address Address::Any(Type type, uint16 port)
		{
			switch (type)
			{
			case Type::IPv4:
			{
				sockaddr_storage storage{ 0 };
				sockaddr_in& addr = reinterpret_cast<sockaddr_in&>(storage);
				addr.sin_addr.s_addr = INADDR_ANY;
				addr.sin_port = htons(port);
				addr.sin_family = AF_INET;
				return Address(storage);
			}
			case Type::IPv6:
			{
				sockaddr_storage storage{ 0 };
				sockaddr_in6& addr = reinterpret_cast<sockaddr_in6&>(storage);
				addr.sin6_addr = in6addr_any;
				addr.sin6_port = htons(port);
				addr.sin6_family = AF_INET6;
				return Address(reinterpret_cast<sockaddr_storage&>(addr));
			}
			default:
				assert(false);
				return Address();
			}
		}

		// Permet de prendre en charge l'adresse IP locale
		Address Address::Loopback(Type type, uint16 port)
		{
			switch (type)
			{
			case Type::IPv4:
			{
				sockaddr_storage storage{ 0 };
				sockaddr_in& addr = reinterpret_cast<sockaddr_in&>(storage);
				addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
				addr.sin_port = htons(port);
				addr.sin_family = AF_INET;
				return Address(storage);
			}
			case Type::IPv6:
			{
				// TODO
			}
			default:
				assert(false);
				return Address();
			}
		}

		std::string Address::address() const
		{
			if (mType == Type::None)
				return "<None>";
			static constexpr int MaxBufferSize = std::max(INET_ADDRSTRLEN, INET6_ADDRSTRLEN);
			char buffer[MaxBufferSize];
			// Use a const_cast because of some Windows API... the object is not changed so it's okay
			if (mStorage.ss_family == AF_INET && inet_ntop(mStorage.ss_family, &reinterpret_cast<sockaddr_in*>(const_cast<sockaddr_storage*>(&mStorage))->sin_addr, buffer, MaxBufferSize) != nullptr)
				return buffer;
			if (mStorage.ss_family == AF_INET6 && inet_ntop(mStorage.ss_family, &reinterpret_cast<sockaddr_in6*>(const_cast<sockaddr_storage*>(&mStorage))->sin6_addr, buffer, MaxBufferSize) != nullptr)
				return buffer;
			return "<Unknown>";
		}

		std::string Address::toString() const
		{
			std::ostringstream str;
			str << address() << ":" << port();
			return str.str();
		}

		bool Address::operator==(const Address& other) const
		{
			if (mType != other.mType)
				return false;
			if (mType == Type::None)
				return true;
			if (mPort != other.mPort)
				return false;
			if (mType == Type::IPv4)
			{
				return memcmp(&mStorage, &(other.mStorage), sizeof(mStorage)) == 0;
			}
			// IpV6
			return memcmp(&reinterpret_cast<const sockaddr_in6&>(mStorage).sin6_addr, &reinterpret_cast<const sockaddr_in6&>(other.mStorage).sin6_addr, sizeof(IN6_ADDR)) == 0;
		}

		bool Address::operator<(const Address& other) const
		{
			if (mType != other.mType)
				return mType < other.mType;
			if (mType == Type::None)
				return false;
			if (mPort != other.mPort)
				return mPort < other.mPort;
			if (mType == Type::IPv4)
			{
				return memcmp(&mStorage, &(other.mStorage), sizeof(mStorage)) < 0;
			}
			// IpV6
			return memcmp(&reinterpret_cast<const sockaddr_in6&>(mStorage).sin6_addr, &reinterpret_cast<const sockaddr_in6&>(other.mStorage).sin6_addr, sizeof(IN6_ADDR)) < 0;
		}

		// connecter un socket � une adresse
		bool Address::connect(SOCKET sckt) const
		{
			return ::connect(sckt, reinterpret_cast<const sockaddr*>(&mStorage), sizeof(mStorage)) == 0;
		}

		// permet d'accepter la connexion entrante sur le socket et r�cup�rer l'adresse du client connect�
		bool Address::accept(SOCKET sckt, SOCKET& newClient)
		{
			sockaddr_storage storage{ 0 };
			sockaddr_in& addr = reinterpret_cast<sockaddr_in&>(storage);
			socklen_t addrlen = sizeof(addr);
			SOCKET newClientSocket = ::accept(sckt, reinterpret_cast<sockaddr*>(&addr), &addrlen);
			if (newClientSocket == INVALID_SOCKET)
			{
				return false;
			}
			set(storage);
			newClient = newClientSocket;
			return true;
		}

		// Associe le socket � une adresse
		bool Address::bind(SOCKET sckt) const
		{
			return ::bind(sckt, reinterpret_cast<const sockaddr*>(&mStorage), sizeof(mStorage)) == 0;
		}

		// permet d'envoyer des donn�es via un socket UDP
		int Address::sendTo(SOCKET sckt, const char* data, size_t datalen) const
		{
			return sendto(sckt, data, static_cast<int>(datalen), 0, reinterpret_cast<const sockaddr*>(&mStorage), sizeof(mStorage));
		}

		// permet de recevoir des donn�es via un socket UDP
		int Address::recvFrom(SOCKET sckt, uint8* buffer, size_t bufferSize)
		{
			sockaddr_storage storage{ 0 };
			sockaddr_in& from = reinterpret_cast<sockaddr_in&>(storage);
			socklen_t fromlen = sizeof(from);
			int ret = recvfrom(sckt, reinterpret_cast<char*>(buffer), static_cast<int>(bufferSize), 0, reinterpret_cast<sockaddr*>(&from), &fromlen);
			if (ret >= 0)
			{
				set(storage);
			}
			return ret;
		}
	}
}