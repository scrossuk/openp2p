#include <string.h>

#include <array>

#include <boost/functional/hash.hpp>

#include <p2p/Crypt/Hash/MD5Stream.hpp>

#include <p2p/Root/NetworkId.hpp>
#include <p2p/Root/Parameters.hpp>

#include <p2p/Transport.hpp>

namespace p2p {

	namespace Root {
	
		NetworkId::NetworkId() {
			data_.fill(0x00);
		}
		
		NetworkId NetworkId::Zero() {
			return NetworkId();
		}
		
		NetworkId NetworkId::FromReader(BlockingReader& reader) {
			NetworkId id;
			reader.readAll(id.data_.data(), id.data_.size());
			return id;
		}
		
		NetworkId NetworkId::Generate(const std::string& networkName) {
			// Perform a hash on the name to
			// generate the network ID.
			Crypt::Hash::MD5Stream hashStream;
			BinaryOStream blockingStream(hashStream);
			blockingStream.writeAll(reinterpret_cast<const uint8_t*>(networkName.c_str()), networkName.size());
			const auto digest = hashStream.calculateDigest();
			assert(digest.size() >= NETWORK_ID_SIZE_BYTES);
			
			NetworkId id;
			memcpy(id.data_.data(), digest.data(), NETWORK_ID_SIZE_BYTES);
			return id;
		}
		
		void NetworkId::writeTo(BlockingWriter& writer) const {
			writer.writeAll(data_.data(), data_.size());
		}
		
		bool NetworkId::operator==(const NetworkId& other) const {
			return data_ == other.data_;
		}
		
		bool NetworkId::operator!=(const NetworkId& other) const {
			return data_ != other.data_;
		}
		
		bool NetworkId::operator<(const NetworkId& other) const {
			return data_ < other.data_;
		}
		
		std::size_t NetworkId::hash() const {
			return boost::hash_range(data_.begin(), data_.end());
		}
		
		namespace {
			
			char getHexDigit(uint8_t value) {
				assert(value < 16);
				if (value < 10) {
					return '0' + value;
				} else {
					return 'A' + (value - 10);
				}
			}
			
		}
		
		std::string NetworkId::hexString() const {
			std::array<char, NETWORK_ID_SIZE_BYTES*2> hexData;
			for (size_t i = 0; i < NETWORK_ID_SIZE_BYTES; i++) {
				const uint8_t value = data_.at(i);
				hexData.at(i * 2 + 0) = getHexDigit((value >> 4) & 15);
				hexData.at(i * 2 + 1) = getHexDigit((value >> 0) & 15);
			}
			return std::string(hexData.data(), hexData.size());
		}
		
	}
	
}

