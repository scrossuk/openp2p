#ifndef P2P_EVENT_UNIONGENERATOR_HPP
#define P2P_EVENT_UNIONGENERATOR_HPP

#include <memory>
#include <vector>

namespace p2p {

	namespace Event {
	
		class Source;
		
		class UnionGenerator {
			public:
				UnionGenerator(std::vector<Source> sourceList);
				~UnionGenerator();
				
				Source eventSource() const;
				
			private:
				// Non-copyable.
				UnionGenerator(const UnionGenerator&) = delete;
				UnionGenerator& operator=(UnionGenerator) = delete;
				
				std::unique_ptr<struct UnionGeneratorImpl> impl_;
				
		};
		
	}
	
}

#endif
