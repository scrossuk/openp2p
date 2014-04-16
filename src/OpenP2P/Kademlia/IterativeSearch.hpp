#ifndef OPENP2P_KADEMLIA_ITERATIVESEARCH_HPP
#define OPENP2P_KADEMLIA_ITERATIVESEARCH_HPP

#include <cstddef>
#include <vector>

#include <OpenP2P/Event/Source.hpp>
#include <OpenP2P/Event/Timer.hpp>
#include <OpenP2P/Event/Wait.hpp>

namespace OpenP2P {

	namespace Kademlia {
	
		template <typename IdType>
		class RPC {
			public:
				virtual Event::Source eventSource() const = 0;
				
				virtual void sendFind(const IdType& destId, const IdType& searchId) = 0;
				
				virtual bool receiveFind(IdType& sourceId, std::vector<IdType>& group) = 0;
				
		};
		
		template <typename IdType, size_t MIN_ITERATION_COUNT = 2, size_t TIMEOUT_MILLISECONDS = 250, size_t PARALLEL_REQUESTS = 3, size_t GROUP_SIZE = 20>
		std::vector<IdType> iterativeSearch(BucketSet& bucketSet, RPC& rpc, const IdType& searchId) {
			SearchQueue<IdType> searchQueue(searchId);
			for (const auto& initialId: bucketSet.getNearest(searchId, GROUP_SIZE)) {
				searchQueue.add(initialId);
			}
			
			size_t activeRequestCount = 0;
			size_t iterationCount = 0;
			
			while (true) {
				// Send out the requests.
				const auto requestGroup = searchQueue.getNearestUnvisited(PARALLEL_REQUESTS);
				for (const auto& requestId: requestGroup) {
					rpc.sendFind(requestId, searchId);
					searchQueue.setVisited(requestId);
					activeRequestCount++;
				}
				
				// Set a timer, after which more requests will be sent.
				Timer timer;
				timer.setMilliseconds(TIMEOUT_MILLISECONDS);
				timer.schedule();
				
				size_t responseCount = 0;
				
				while (activeRequestCount > 0 && !timer.isActive()) {
					IdType sourceId;
					std::vector<IdType> receivedGroup;
					if (rpc.receiveFind(sourceId, receivedGroup)) {
						activeRequestCount--;
						for (const auto& receivedId: receivedGroup) {
							bucketSet.add(receivedId);
							searchQueue.add(receiveId);
						}
					}
					
					Event::Wait({ rpc.eventSource(), timer });
				}
				
				iterationCount++
				
				if (searchQueue.isNearestVisited() && iterationCount >= MIN_ITERATION_COUNT) {
					// Didn't make any progress and already completed
					// required number of iterations, so end here.
					break;
				}
			}
			
			return searchQueue.getNearest(GROUP_SIZE);
		}
		
	}
	
}

#endif
