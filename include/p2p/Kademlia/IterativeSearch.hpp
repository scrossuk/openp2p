#ifndef P2P_KADEMLIA_ITERATIVESEARCH_HPP
#define P2P_KADEMLIA_ITERATIVESEARCH_HPP

#include <cstddef>
#include <vector>

#include <p2p/Event/Source.hpp>
#include <p2p/Event/Timer.hpp>
#include <p2p/Event/Wait.hpp>

#include <p2p/Kademlia/BucketSet.hpp>
#include <p2p/Kademlia/SearchQueue.hpp>

namespace p2p {

	namespace Kademlia {
	
		template <typename IdType>
		class RPCSocket {
			public:
				virtual Event::Source eventSource() const = 0;
				
				virtual RPC::Operation<std::vector<IdType>> performFind(const IdType& destId, const IdType& searchId) = 0;
				
		};
		
		template <typename IdType, size_t MIN_ITERATION_COUNT = 2, size_t TIMEOUT_MILLISECONDS = 250, size_t PARALLEL_REQUESTS = 3, size_t GROUP_SIZE = 20>
		std::vector<IdType> iterativeSearch(BucketSet& bucketSet, RPCSocket<IdType>& socket, const IdType& searchId) {
			SearchQueue<IdType> searchQueue(searchId);
			for (const auto& initialId: bucketSet.getNearest(searchId, GROUP_SIZE)) {
				searchQueue.add(initialId);
			}
			
			std::list<RPC::Operation<std::vector<IdType>>> activeRequests;
			
			size_t iterationCount = 0;
			
			while (true) {
				// Send out the requests.
				const auto requestGroup = searchQueue.getNearestUnvisited(PARALLEL_REQUESTS);
				for (const auto& requestId: requestGroup) {
					auto rpc = socket.performFind(requestId, searchId);
					activeRequests.push_back(std::move(rpc));
					searchQueue.setVisited(requestId);
				}
				
				// Set a timer, after which more requests will be sent.
				Timer timer;
				timer.setMilliseconds(TIMEOUT_MILLISECONDS);
				timer.schedule();
				
				size_t responseCount = 0;
				
				while (!activeRequests.empty() && !timer.isActive()) {
					std::vector<Event::Source> eventSources;
					eventSources.reserve(activeRequests.size() + 1);
					
					for (auto iterator = activeRequests.begin(); iterator != activeRequests.end(); ) {
						const auto& rpc = *iterator;
						if (rpc.isComplete()) {
							auto nextIterator = iterator;
							++nextIterator;
							
							const auto& receivedGroup = rpc.get();
							for (const auto& receivedId: receivedGroup) {
								bucketSet.add(receivedId);
								searchQueue.add(receiveId);
							}
							
							activeRequests.erase(iterator);
							
							iterator = std::move(nextIterator);
						} else {
							eventSources.push_back(rpc.eventSource());
							++iterator;
						}
					}
					
					eventSources.push_back(timer.eventSource());
					
					Event::Wait(eventSources);
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
