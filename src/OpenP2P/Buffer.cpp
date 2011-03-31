#include <sstream>
#include <string>
#include <iostream>
#include <stdint.h>
#include <cstddef>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <OpenP2P/Buffer.hpp>

namespace OpenP2P {

	namespace {

		class BufferNode: public BufferTree {
			public:
				BufferNode(boost::shared_ptr<BufferTree> left, boost::shared_ptr<BufferTree> right) : left_(left), right_(right) {
					size_ = left->size() + right->size();
				}

				boost::shared_ptr<BufferTree> left() {
					return left_;
				}

				boost::shared_ptr<BufferTree> right() {
					return right_;
				}

				const uint8_t * data() {
					return 0;
				}

				std::size_t size() {
					return size_;
				}

			private:
				boost::shared_ptr<BufferTree> left_;
				boost::shared_ptr<BufferTree> right_;
				std::size_t size_;

		};

		class BufferBlock{
			public:
				BufferBlock() : data_(0), size_(0) { }

				BufferBlock(const uint8_t * data, std::size_t size) : size_(size) {
					data_ = new uint8_t[size];

					for (std::size_t i = 0; i < size_; ++i) {
						data_[i] = data[i];
					}
				}

				~BufferBlock() {
					if (data_ != 0) {
						delete[] data_;
					}
				}

				const uint8_t * data() {
					return data_;
				}

				std::size_t size() {
					return size_;
				}

			private:
				uint8_t * data_;
				std::size_t size_;

		};

		class BufferLeaf: public BufferTree, public boost::enable_shared_from_this<BufferLeaf> {
			public:
				BufferLeaf(boost::shared_ptr<BufferBlock> block) : block_(block) {
					start_ = 0;
					end_ = block_->size();
				}

				BufferLeaf(boost::shared_ptr<BufferBlock> block, std::size_t start, std::size_t end) : block_(block) {
					start_ = start;
					end_ = end;
				}

				boost::shared_ptr<BufferTree> left() {
					return shared_from_this();
				}

				boost::shared_ptr<BufferTree> right() {
					return shared_from_this();
				}

				const uint8_t * data() {
					return block_->data() + start_;
				}

				std::size_t size() {
					return end_ - start_;
				}

			private:
				boost::shared_ptr<BufferBlock> block_;
				std::size_t start_, end_;

		};

	}

	Buffer::Buffer()
			: tree_(boost::shared_ptr<BufferTree>(new BufferLeaf(boost::shared_ptr<BufferBlock>(new BufferBlock())))) { }

	Buffer::Buffer(const uint8_t * data, std::size_t size)
			: tree_(boost::shared_ptr<BufferTree>(new BufferLeaf(boost::shared_ptr<BufferBlock>(new BufferBlock(data, size))))) { }

	Buffer::Buffer(const Buffer& Buffer)
			: tree_(Buffer.tree_) { }

	Buffer::Buffer(const Buffer& a, const Buffer& b)
			: tree_(boost::shared_ptr<BufferTree>(new BufferNode(a.tree_, b.tree_))) { }

	Buffer Buffer::operator+(const Buffer& buffer) const {
		return Buffer(*this, buffer);
	}

	boost::shared_ptr<BufferTree> Buffer::tree() const {
		return tree_;
	}

	std::size_t Buffer::size() const {
		return tree_->size();
	}

	std::string getBufferString(const boost::shared_ptr<BufferTree>& ptr){
		std::ostringstream stream;
		if(ptr->left() == ptr){
			stream << "Leaf(" << ptr->size() << ")";
		}else{
			stream << "Node(" << getBufferString(ptr->left()) << ", " << getBufferString(ptr->right()) << ")";
		}
		return stream.str();
	}

	void printBuffer(const Buffer& buffer){
		std::cout << getBufferString(buffer.tree()) << std::endl;
	}

}
