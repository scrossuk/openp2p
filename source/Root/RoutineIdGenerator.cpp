#include <stdint.h>

#include <mutex>

#include <p2p/Root/RoutineIdGenerator.hpp>

namespace p2p {

	namespace Root {
	
		RoutineIdGenerator::RoutineIdGenerator(uint32_t initialValue)
			: nextId_(initialValue) { }
		
		uint32_t RoutineIdGenerator::generateId() {
			std::lock_guard<std::mutex> lock(mutex_);
			return nextId_++;
		}
		
	}
	
}

