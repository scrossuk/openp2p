#ifndef OPENP2P_FOLDERSYNC_BLOCK_HPP
#define OPENP2P_FOLDERSYNC_BLOCK_HPP

#include <stdint.h>

#include <vector>

#include <OpenP2P/FolderSync/Constants.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		class Block {
			public:
				inline Block() 
					: data_(BLOCK_SIZE, 0x00) { }
				
				inline uint8_t* data() {
					return data_.data();
				}
				
				inline const uint8_t* data() const {
					return data_.data();
				}
				
				inline size_t size() const {
					return BLOCK_SIZE;
				}
				
			private:
				std::vector<uint8_t> data_;
			
		};
		
	}
	
}

#endif
