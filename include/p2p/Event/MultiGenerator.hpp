#ifndef P2P_EVENT_MULTIGENERATOR_HPP
#define P2P_EVENT_MULTIGENERATOR_HPP

#include <memory>

#include <p2p/Event/Event_api.hpp>

namespace p2p {

	namespace Event {
	
		class Source;
		
		class EVENT_OPENP2P_API MultiGenerator {
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
