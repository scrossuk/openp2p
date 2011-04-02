#ifndef OPENP2P_KADEMLIA_IDGENERATOR_HPP
#define OPENP2P_KADEMLIA_IDGENERATOR_HPP

#include <stdint.h>
#include <OpenP2P/IdGenerator.hpp>
#include <OpenP2P/RootNetwork/Id.hpp>
#include <OpenP2P/RootNetwork/Parameters.hpp>

namespace OpenP2P{

	namespace RootNetwork{

		//Simple increment ID generator
		class IdGenerator : public OpenP2P::IdGenerator<Id>{
			public:
				inline IdGenerator() : id_(zeroId()){ }

				inline Id generate(){
					for(unsigned int i = IdSize - 1; i >= 0; i--){
						if(id_.data[i] == 255){
							id_.data[i] = 0;
						}else{
							id_.data[i]++;
							return id_;
						}
					}
					return id_;
				}

			private:
				Id id_;

		};

	}

}

#endif
