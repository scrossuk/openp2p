#include <assert.h>
#include <stdint.h>
#include <string.h>

#include <array>
#include <functional>

#include <boost/functional/hash.hpp>

#include <p2p/Crypt/Hash/MD5Stream.hpp>

#include <p2p/FolderSync/Block.hpp>
#include <p2p/FolderSync/BlockId.hpp>

#include <p2p/Transport.hpp>

namespace p2p {
	
	namespace FolderSync {
		
		BlockId::BlockId() {
			data_.fill(0x00);
		}
		
		BlockId BlockId::Zero() {
			return BlockId();
		}
		
		BlockId BlockId::FromReader(BlockingReader& reader) {
			BlockId blockId;
			reader.readAll(blockId.data_.data(), blockId.data_.size());
			return blockId;
		}
		
		BlockId BlockId::Generate(const Block& data) {
			// Perform a hash on the block to
			// generate its ID.
			Crypt::Hash::MD5Stream hashStream;
			BinaryOStream blockingStream(hashStream);
			blockingStream.writeAll(data.data(), data.size());
			const auto digest = hashStream.calculateDigest();
			assert(digest.size() == BLOCK_ID_SIZE);
			
			BlockId id;
			memcpy(id.data_.data(), digest.data(), BLOCK_ID_SIZE);
			return id;
		}
		
		void BlockId::writeTo(BlockingWriter& writer) const {
			writer.writeAll(data_.data(), data_.size());
		}
		
		bool BlockId::operator==(const BlockId& other) const {
			return data_ == other.data_;
		}
		
		bool BlockId::operator<(const BlockId& other) const {
			return data_ < other.data_;
		}
		
		std::size_t BlockId::hash() const {
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
		
		std::string BlockId::hexString() const {
			std::array<char, BLOCK_ID_SIZE*2> hexData;
			for (size_t i = 0; i < BLOCK_ID_SIZE; i++) {
				const uint8_t value = data_.at(i);
				hexData.at(i * 2 + 0) = getHexDigit((value >> 4) & 15);
				hexData.at(i * 2 + 1) = getHexDigit((value >> 0) & 15);
			}
			return std::string(hexData.data(), hexData.size());
		}
		
	}
	
}

