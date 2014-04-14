#ifndef OPENP2P_EVENT_MULTIGENERATOR_HPP
#define OPENP2P_EVENT_MULTIGENERATOR_HPP

#include <memory>

namespace OpenP2P {

	namespace Event {
	
		class Source;
		
		class MultiGenerator {
			public:
				MultiGenerator(Source source);
				~MultiGenerator();
				
				size_t addClient();
				
				void removeClient(size_t clientId);
				
				Source eventSource(size_t clientId) const;
				
			private:
				// Non-copyable.
				MultiGenerator(const MultiGenerator&) = delete;
				MultiGenerator& operator=(MultiGenerator) = delete;
				
				std::unique_ptr<struct MultiGeneratorImpl> impl_;
				
		};
		
	}
	
}

#endif
