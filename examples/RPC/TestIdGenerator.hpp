#include <p2p.hpp>

class TestIdGenerator: public p2p::IdGenerator<uint32_t> {
	public:
		TestIdGenerator() : id_(0) { }
		
		uint32_t generate() {
			return id_++;
		}
		
	private:
		uint32_t id_;
		
};

