#include <stdint.h>
#include <cstddef>
#include <stack>
#include <boost/shared_ptr.hpp>
#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/BufferIterator.hpp>
#include <OpenP2P/WaitHandler.hpp>

namespace OpenP2P {

	BufferIterator::BufferIterator() {
		seek(0);
	}

	BufferIterator::BufferIterator(const Buffer& buffer, std::size_t offset) : buffer_(buffer) {
		seek(offset);
	}

	std::size_t BufferIterator::position() {
		return position_;
	}

	std::size_t BufferIterator::remaining() {
		return buffer_.size() - position_;
	}

	void BufferIterator::set(const Buffer& buffer) {
		buffer_ = buffer;
		seek(0);
	}

	bool BufferIterator::seek(std::size_t offset) {
		if (offset > buffer_.size()) {
			return false;
		}

		position_ = offset;

		while(!stack_.empty()){
			stack_.pop();
		}

		boost::shared_ptr<BufferTree> node = buffer_.tree();
		boost::shared_ptr<BufferTree> left = node->left();

		while (node != left) {
			std::size_t midpoint = left->size();
			stack_.push(node);
			if (offset < midpoint) {
				node = left;
			}
			else {
				node = node->right();
				offset -= midpoint;
			}
			left = node->left();
		}

		stack_.push(node);

		offset_ = offset;
		return true;
	}

	std::size_t BufferIterator::readSome(uint8_t * data, std::size_t dataSize, WaitHandler) {
		const uint8_t * readData = stack_.top()->data() + offset_;
		std::size_t maxReadSize = stack_.top()->size() - offset_;

		std::size_t readSize = (dataSize < maxReadSize) ? dataSize : maxReadSize;

		for (std::size_t i = 0; i < readSize; ++i) {
			data[i] = readData[i];
		}

		position_ += readSize;
		offset_ += readSize;

		if (readSize == maxReadSize) {
			moveSuccessor();
		}

		return readSize;
	}

	bool BufferIterator::moveSuccessor() {
		if (stack_.size() < 2) {
			seek(buffer_.size());
			return false;
		}

		boost::shared_ptr<BufferTree> child = stack_.top();

		stack_.pop();

		boost::shared_ptr<BufferTree> node = stack_.top();

		while (child == node->right()) {
			stack_.pop();

			if (stack_.empty()) {
				seek(buffer_.size());
				return false;
			}

			child = node;
			node = stack_.top();
		}

		node = node->right();
		child = node->left();

		while (node != child) {
			stack_.push(node);
			node = child;
			child = node->left();
		}

		offset_ = 0;

		stack_.push(node);
		return true;
	}

	OStream& operator<<(OStream& stream, const Buffer& buffer){
		BufferIterator iterator(buffer);

		uint32_t bufferSize = buffer.size();
		stream << bufferSize;

		uint8_t data[1024];
		std::size_t size = 1;
		while((size = iterator.readSome(data, 1024)) != 0){
			stream.write(data, size);
		}

		return stream;
	}

}

