#ifndef OPENP2P_BLOCK_HPP
#define OPENP2P_BLOCK_HPP

#include <stdint.h>
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <boost/shared_ptr.hpp>

#include <OpenP2P/Lock.hpp>
#include <OpenP2P/Mutex.hpp>

namespace OpenP2P{

	const std::size_t BlockSize = 1024;

	class MemBlock{
		public:
			inline MemBlock() : size_(0){ }
		
			inline MemBlock(std::size_t dataSize)
				: size_(dataSize){ }
		
			inline MemBlock(const uint8_t * dataPtr, std::size_t dataSize)
				: size_(dataSize)
				{
					memcpy((void *) data_, (const void *) dataPtr, dataSize);
				}
				
			inline std::size_t size() const{
				Lock lock(mutex_);
				return size_;
			}
			
			inline bool expand(std::size_t currentSize, std::size_t expandSize){
				Lock lock(mutex_);
				if(size_ == currentSize){
					size_ = std::min(size_ + expandSize, BlockSize);
					return true;
				}else{
					return false;
				}
			}
			
			inline uint8_t * get(){
				return data_;
			}
			
			inline const uint8_t * get() const{
				return data_;
			}
				
			inline uint8_t& operator[](std::size_t offset){
				return data_[offset];
			}
			
			inline uint8_t operator[](std::size_t offset) const{
				return data_[offset];
			}
			
		private:
			uint8_t data_[BlockSize];
			std::size_t size_;
			mutable Mutex mutex_;
			
	};

	class Block{
		public:
			inline Block() : offset_(0), size_(0){ }
			
			inline Block(const uint8_t * data, std::size_t size)
				: offset_(0), size_(std::min(size, BlockSize)),
				mem_(new MemBlock(data, size_)){ }
				
			inline Block(MemBlock * mem)
				: offset_(0), size_(mem->size()), mem_(mem){ }
				
			inline Block(const boost::shared_ptr<MemBlock>& mem)
				: offset_(0), size_(mem->size()), mem_(mem){ }
				
			inline Block(MemBlock * mem, std::size_t offset, std::size_t size)
				: offset_(offset), size_(size), mem_(mem){ }
				
			inline Block(const boost::shared_ptr<MemBlock>& mem, std::size_t offset, std::size_t size)
				: offset_(offset), size_(size), mem_(mem){ }
				
			inline Block& operator=(const Block& block){
				if(this != &block){
					offset_ = block.offset_;
					size_ = block.size_;
					mem_ = block.mem_;
				}
				return *this;
			}
			
			inline const uint8_t * get() const{
				if(mem_.get() == 0){
					return 0;
				}else{
					return mem_->get() + offset_;
				}
			}
			
			inline uint8_t operator[](std::size_t offset) const{
				if(mem_.get() == 0){
					return 0;
				}else{
					return (*mem_)[offset_ + offset];
				}
			}
			
			inline std::size_t offset() const{
				return offset_;
			}
			
			inline std::size_t size() const{
				return size_;
			}
			
			inline Block substr(std::size_t startPos, std::size_t endPos = BlockSize) const{
				std::size_t actualOffset = std::min(startPos, size_);
				std::size_t actualSize = std::min(std::max(actualOffset, endPos), size_) - actualOffset;
				if(actualSize == 0){
					return Block();
				}else if(actualSize == size_){
					return *this;
				}else{
					return Block(mem_, offset_ + actualOffset, actualSize);
				}
			}
			
			inline Block append(const uint8_t * appendData, std::size_t appendSize) const{
				if(appendSize == 0){
					return *this;
				}else if(size_ == 0){
					return Block(appendData, appendSize);
				}else{
					std::size_t actualAppendSize = std::min(appendSize, BlockSize - size_ - offset_);
					if(mem_->expand(offset_ + size_, actualAppendSize)){
						memcpy((void *) (mem_->get() + offset_ + size_), (const void *) appendData, actualAppendSize);
						return Block(mem_, offset_, size_ + actualAppendSize);
					}else{
						return Block(mem_->get() + offset_, size_).append(appendData, appendSize);
					}
				}
			}
			
		private:
			std::size_t offset_, size_;
			boost::shared_ptr<MemBlock> mem_;
		
	};

}

#endif
