#ifndef OPENP2P_KADEMLIA_IDGENERATOR_HPP
#define OPENP2P_KADEMLIA_IDGENERATOR_HPP

#include <stdint.h>
#include <OpenP2P/Generator.hpp>
#include <OpenP2P/Kademlia/Id.hpp>

namespace OpenP2P{

	namespace Kademlia{

		template <std::size_t IdSize>
		class IdGenerator : public Generator< Id<IdSize> >{
			public:
				IdGenerator() : id_(ZeroId<IdSize>()){ }

				Id<IdSize> generate(){
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
				Id<IdSize> id_;

		};

	}

}

#endif
