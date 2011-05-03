#include <sstream>
#include <string>
#include <iostream>
#include <stdint.h>
#include <cstddef>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <OpenP2P/Buffer.hpp>

namespace OpenP2P {

	/*Buffer::Buffer(){ }

	Buffer::Buffer(const uint8_t * data, std::size_t size)
			: data_(data, data + size){ }

	Buffer::Buffer(const Buffer& buffer)
			: data_(buffer.data_){ }

	std::vector<uint8_t>& Buffer::data(){
		return data_;
	}

	std::size_t Buffer::size() const {
		return data_.size();
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
	}*/

}
