#ifndef P2P_ROOTNETWORK_ROUTINEIDGENERATOR_HPP
#define P2P_ROOTNETWORK_ROUTINEIDGENERATOR_HPP

#include <stdint.h>

#include <mutex>
#include <p2p/Root/Root_api.hpp>

namespace p2p {

	namespace Root {
		
		/**
		 * \brief Routine ID Generator.
		 * 
		 * Thread safe class to generate routine ids. These
		 * are just 32 bit unsigned values that automatically
		 * roll over from (2^32 - 1) to 0 (it's assumed routines
		 * will have ended by the time their ID is re-used).
		 */
		class ROOT_OPENP2P_API RoutineIdGenerator {
			public:
				RoutineIdGenerator(uint32_t initialValue = 0);
				
				uint32_t generateId();
				
			private:
				// Non-copyable.
				RoutineIdGenerator(const RoutineIdGenerator&) = delete;
				RoutineIdGenerator& operator=(const RoutineIdGenerator&) = delete;
				
				std::mutex mutex_;
				uint32_t nextId_;
				
		};
		
	}
	
}

#endif
