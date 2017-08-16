#include <cstddef>
#include <map>
#include <mutex>

#include <p2p/Event/Generator.hpp>
#include <p2p/Event/MultiGenerator.hpp>
#include <p2p/Event/Source.hpp>
#include <p2p/Event/SourceImpl.hpp>

namespace p2p {

	namespace Event {
	
		struct MultiGeneratorImpl: public Receiver {
			public:
				MultiGeneratorImpl(Source source)
					: sourceImpl_(source.impl()), nextId_(0ull) {
						sourceImpl_->addReceiver(*this);
					}
				
				~MultiGeneratorImpl() {
					sourceImpl_->removeReceiver(*this);
				}
				
				void notify() {
					std::lock_guard<std::mutex> lock(mutex_);
					for (auto& clientGeneratorPair: clientGenerators_) {
						clientGeneratorPair.second.activate();
					}
				}
				
				size_t addClient() {
					std::lock_guard<std::mutex> lock(mutex_);
					const auto clientId = nextId_++;
					const bool initialState = true;
					clientGenerators_.insert(std::make_pair(clientId, Generator(initialState)));
					return clientId;
				}
				
				void removeClient(size_t clientId) {
					std::lock_guard<std::mutex> lock(mutex_);
					clientGenerators_.erase(clientId);
				}
				
				Source eventSource(size_t clientId) const {
					std::lock_guard<std::mutex> lock(mutex_);
					return clientGenerators_.at(clientId).eventSource();
				}
				
			private:
				mutable std::mutex mutex_;
				SourceImpl* sourceImpl_;
				size_t nextId_;
				std::map<size_t, Generator> clientGenerators_;
				
		};
		
		MultiGenerator::MultiGenerator(Source source)
			: impl_(new MultiGeneratorImpl(source)) { }
			
		MultiGenerator::~MultiGenerator() { }
		
		size_t MultiGenerator::addClient() {
			return impl_->addClient();
		}
		
		void MultiGenerator::removeClient(size_t clientId) {
			impl_->removeClient(clientId);
		}
		
		Source MultiGenerator::eventSource(size_t clientId) const {
			return impl_->eventSource(clientId);
		}
		
	}
	
}

