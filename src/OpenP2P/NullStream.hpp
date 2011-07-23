#ifndef OPENP2P_NULLSTREAM_HPP
#define OPENP2P_NULLSTREAM_HPP

#include <cstring>

#include <OpenP2P/Stream.hpp>

namespace OpenP2P{

	class NullIStream: public IStream{
		public:
			inline Future<Block> readSome(){
				// Return a block of zeroes.
				MemBlock * memBlock = new MemBlock(BlockSize);
				memset((void *) memBlock->get(), 0, BlockSize);
				return Block(memBlock);
			}

	};

	class NullOStream: public OStream{
		public:
			inline Future<std::size_t> writeSome(const Block& block){
				return block.size();
			}

	};

	class NullIOStream: public NullIStream, public NullOStream{ };

}

#endif
