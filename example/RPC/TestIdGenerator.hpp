#include <OpenP2P.hpp>

class TestIdGenerator: public OpenP2P::IdGenerator<uint32_t> {
	public:
		TestIdGenerator() : id_(0) { }
		
		uint32_t generate() {
			return id_++;
		}
		
	private:
		uint32_t id_;
		
};

