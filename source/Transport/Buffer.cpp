#include <p2p/Transport/Buffer.hpp>

namespace p2p {

	/*Buffer::Buffer(){ }
	
	Buffer::Buffer(const uint8_t * data, size_t size)
			: data_(data, data + size){ }
	
	Buffer::Buffer(const Buffer& buffer)
			: data_(buffer.data_){ }
	
	std::vector<uint8_t>& Buffer::data(){
		return data_;
	}
	
	size_t Buffer::size() const {
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
