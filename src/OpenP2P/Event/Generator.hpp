#ifndef OPENP2P_EVENT_GENERATOR_HPP
#define OPENP2P_EVENT_GENERATOR_HPP

#include <memory>

namespace OpenP2P {

	namespace Event {
	
		class Source;
		
		class Generator {
			public:
				Generator();
				~Generator();
				
				void activate();
				
				Source eventSource() const;
				
			private:
				// Non-copyable.
				Generator(const Generator&) = delete;
				Generator& operator=(Generator) = delete;
				
				std::unique_ptr<struct GeneratorImpl> impl_;
				
		};
		
	}
	
}

#endif
