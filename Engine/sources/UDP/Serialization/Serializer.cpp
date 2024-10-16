#include "UDP/Serialization/Serializer.h"
#include "UDP/Serialization/Convert.h"
#include "UDP/Serialization/Serialization.h"
#include "UDP/Utils/Utils.h"

#include <algorithm>

namespace Bousk
{
	namespace Serialization
	{
		bool Serializer::write(const Serializable& serializable)
		{
			return serializable.write(*this);
		}

		bool Serializer::writeBits(const uint8* const buffer, const uint8 buffersize, const uint8 nbBits)
		{
			static_assert(CHAR_BIT == 8, "");
			uint8 totalWrittenBits = 0;
			// buffer here is in network/big endian, so bytes to write must be read right (buffer + buffersize - 1) to left (buffer)
			for (uint8 readingBytesOffset = 1; readingBytesOffset <= buffersize && totalWrittenBits < nbBits; ++readingBytesOffset)
			{
				const uint8 srcByte = *(buffer + buffersize - readingBytesOffset);
				const uint8 bitsToWrite = static_cast<uint8>(std::min(8, nbBits - totalWrittenBits));
				uint8 writtenBits = 0;
				if (mUsedBits)
				{
					// We have an existing byte to pack data to
					const uint8 remainingBitsInCurrentByte = 8 - mUsedBits;
					const uint8 nbBitsToPack = std::min(bitsToWrite, remainingBitsInCurrentByte);
					// Extract bits from the right
					const uint8 rightBitsToPack = srcByte & Utils::CreateRightBitsMask(nbBitsToPack);
					// Align those bits on the left to pack to existing bits in buffer
					const uint8 bitsShiftToAlignLeft = remainingBitsInCurrentByte - nbBitsToPack;
					const uint8 leftAlignedBits = rightBitsToPack << bitsShiftToAlignLeft;
					mBuffer.back() |= leftAlignedBits;
					writtenBits += nbBitsToPack;
				}
				const uint8 remainingBits = bitsToWrite - writtenBits;
				if (remainingBits)
				{
					// Extract bits to serialize
					const uint8 leftBitsToPack = srcByte & Utils::CreateBitsMask(remainingBits, writtenBits);
					// Align bits to the left on the new byte
					const uint8 bitsShiftToAlignLeft = 8 - writtenBits - remainingBits;
					const uint8 leftAlignedBits = leftBitsToPack << bitsShiftToAlignLeft;
					// Add those bits as a new byte to the buffer, they are aligned on the left in new byte
					mBuffer.push_back(leftAlignedBits);
					writtenBits += remainingBits;
				}
				// Update our counters
				totalWrittenBits += writtenBits;
				mUsedBits += writtenBits;
				mUsedBits %= 8;
			}
			return totalWrittenBits == nbBits;
		}

		bool Serializer::write(const uint8 data, const uint8 minValue, const uint8 maxValue)
		{
			assert(minValue < maxValue);
			assert(minValue <= data && data <= maxValue);
			const uint8 rangedData = data - minValue;
			const uint8 range = maxValue - minValue;
			return writeBits(&rangedData, 1, Utils::CountNeededBits(range));
		}
		bool Serializer::write(uint16 data, uint16 minValue, uint16 maxValue)
		{
			assert(minValue < maxValue);
			assert(minValue <= data && data <= maxValue);
			const uint16 rangedData = data - minValue;
			const uint16 range = maxValue - minValue;
			uint8 conv[2];
			Conversion::ToNetwork(rangedData, conv);
			return writeBits(conv, 2, Utils::CountNeededBits(range));
		}
		bool Serializer::write(const uint32 data, const uint32 minValue, const uint32 maxValue)
		{
			assert(minValue < maxValue);
			assert(minValue <= data && data <= maxValue);
			const uint32 rangedData = data - minValue;
			const uint32 range = maxValue - minValue;
			uint8 conv[4];
			Conversion::ToNetwork(rangedData, conv);
			return writeBits(conv, 4, Utils::CountNeededBits(range));
		}
		bool Serializer::write(uint64 data, uint64 minValue, uint64 maxValue)
		{
			assert(minValue < maxValue);
			assert(minValue <= data && data <= maxValue);
			const uint64 rangedData = data - minValue;
			const uint64 range = maxValue - minValue;
			uint8 conv[8];
			Conversion::ToNetwork(rangedData, conv);
			return writeBits(conv, 8, Utils::CountNeededBits(range));
		}

		bool Serializer::write(const int8 data, const int8 minValue, const int8 maxValue)
		{
			static_assert(sizeof(int8) == sizeof(uint8), "");
			assert(minValue < maxValue);
			assert(minValue <= data && data <= maxValue);
			const uint8 rangedData = static_cast<uint8>(data - minValue);
			const uint8 range = static_cast<uint8>(maxValue - minValue);
			return write(rangedData, static_cast<uint8>(0), range);
		}
		bool Serializer::write(const int16 data, int16 minValue, int16 maxValue)
		{
			static_assert(sizeof(int16) == sizeof(uint16), "");
			assert(minValue < maxValue);
			assert(minValue <= data && data <= maxValue);
			const uint16 rangedData = static_cast<uint16>(data - minValue);
			const uint16 range = static_cast<uint16>(maxValue - minValue);
			return write(rangedData, static_cast<uint16>(0), range);
		}
		bool Serializer::write(const int32 data, const int32 minValue, const int32 maxValue)
		{
			static_assert(sizeof(int32) == sizeof(uint32), "");
			assert(minValue < maxValue);
			assert(minValue <= data && data <= maxValue);
			const uint32 rangedData = static_cast<uint32>(data - minValue);
			const uint32 range = static_cast<uint32>(maxValue - minValue);
			return write(rangedData, static_cast<uint32>(0), range);
		}
		bool Serializer::write(int64 data, int64 minValue, int64 maxValue)
		{
			static_assert(sizeof(int64) == sizeof(uint64), "");
			assert(minValue < maxValue);
			assert(minValue <= data && data <= maxValue);
			const uint64 rangedData = static_cast<uint64>(data - minValue);
			const uint64 range = static_cast<uint64>(maxValue - minValue);
			return write(rangedData, static_cast<uint64>(0), range);
		}

		#if BOUSKNET_ALLOW_FLOAT32_SERIALIZATION == BOUSKNET_SETTINGS_ENABLED
			bool Serializer::write(const float32 data)
			{
				uint32 conv;
				Conversion::ToNetwork(data, conv);
				return writeBits(reinterpret_cast<const uint8*>(&conv), 4, 32);
			}
		#endif // BOUSKNET_ALLOW_FLOAT32_SERIALIZATION == BOUSKNET_SETTINGS_ENABLED
	}
}