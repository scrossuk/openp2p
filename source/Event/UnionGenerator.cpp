#include <cstddef>
#include <vector>

#include <p2p/Event/Generator.hpp>
#include <p2p/Event/Source.hpp>
#include <p2p/Event/UnionGenerator.hpp>
#include <p2p/Event/SourceImpl.hpp>

namespace p2p {

	namespace Event {
	
		struct UnionGeneratorImpl: public Receiver {
			public:
				UnionGeneratorImpl(std::vector<Source> sourceList)
					: sourceList_(std::move(sourceList)) {
						for (auto source: sourceList_) {
							source.impl()->addReceiver(*this);
						}
					}
				
				~UnionGeneratorImpl() {
					for (size_t i = 0; i < sourceList_.size(); i++) {
						sourceList_.at(sourceList_.size() - 1 - i).impl()->removeReceiver(*this);
					}
				}
				
				void notify() {
					clientGenerator_.activate();
				}
				
				Source eventSource() const {
					return clientGenerator_.eventSource();
				}
				
			private:
				std::vector<Source> sourceList_;
				Generator clientGenerator_;
				
		};
		
		UnionGenerator::UnionGenerator(std::vector<Source> sourceList)
			: impl_(new UnionGeneratorImpl(std::move(sourceList))) { }
			
		UnionGenerator::~UnionGenerator() { }
		
		Source UnionGenerator::eventSource() const {
			return impl_->eventSource();
		}
		
	}
	
}

