#include <utility>

#include <OpenP2P/Block.hpp>
#include <OpenP2P/BufferedStream.hpp>
#include <OpenP2P/Future.hpp>
#include <OpenP2P/Stream.hpp>

#include <OpenP2P/OFTorrent/XORStream.hpp>

namespace OpenP2P{

	namespace OFTorrent{
	
		namespace{
		
			Block xorBlocks(const std::pair<Block, Block>& blocks){
				const Block& block0 = blocks.first;
				const Block& block1 = blocks.second;
				std::size_t minSize = std::min(block0.size(), block1.size());
				MemBlock * memBlock = new MemBlock(minSize);
				for(std::size_t pos = 0; pos < minSize; ++pos){
					(*memBlock)[pos] = block0[pos] ^ block1[pos];
				}
				return Block(memBlock);
			}
		
		}

		XORIStream::XORIStream(IStream& source0, IStream& source1) : buffered0_(source0), buffered1_(source1){ }

		Future<Block> XORIStream::readSome(){
			if(future_.isReady()){
				Block block = future_.get();
				
				buffered0_.consume(block.size());
				buffered1_.consume(block.size());
				
				Future<Block> read0 = buffered0_.readSome();
				Future<Block> read1 = buffered1_.readSome();
				
				return future_ = read0.lateJoin(read1).compose<Block>(xorBlocks);
			}else{
				return future_;
			}
		}

	}

}

