#include <assert.h>
#include <stdint.h>
#include <string.h>

#include <array>

#include <OpenP2P/Crypt/Hash/MD5Stream.hpp>
#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockId.hpp>
#include <OpenP2P/Stream/BinaryStream.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		BlockId::BlockId() {
			(void) memset(data_.data(), 0, data_.size());
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
		
		bool BlockId::operator==(const BlockId& other) const {
			return data_ == other.data_;
		}
		
		bool BlockId::operator<(const BlockId& other) const {
			return data_ < other.data_;
		}
		
	}
	
}

