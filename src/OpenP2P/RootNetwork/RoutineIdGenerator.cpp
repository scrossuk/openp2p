#include <stdint.h>

#include <mutex>

#include <OpenP2P/RootNetwork/RoutineIdGenerator.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		RoutineIdGenerator::RoutineIdGenerator(uint32_t initialValue)
			: nextId_(initialValue) { }
		
		uint32_t RoutineIdGenerator::generateId() {
			std::lock_guard<std::mutex> lock(mutex_);
			return nextId_++;
		}
		
	}
	
}

