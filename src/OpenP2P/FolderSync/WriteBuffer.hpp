#ifndef OPENP2P_FOLDERSYNC_WRITEBUFFER_HPP
#define OPENP2P_FOLDERSYNC_WRITEBUFFER_HPP

#include <stdint.h>

#include <map>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockStore.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		class WriteBuffer {
			public:
				WriteBuffer(BlockStore& blockStore);
				~WriteBuffer();
				
				bool hasChanges() const;
				
				Block& getWriteBlock(size_t position);
				
				void flush();
				
			private:
				// Non-copyable.
				WriteBuffer(const WriteBuffer&) = delete;
				WriteBuffer& operator=(WriteBuffer) = delete;
				
				BlockStore& blockStore_;
				std::map<size_t, Block> blockMap_;
				
		};
		
	}
	
}

#endif
