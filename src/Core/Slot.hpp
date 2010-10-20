#ifndef OPENP2P_CORE_SLOT_HPP
#define OPENP2P_CORE_SLOT_HPP

namespace openp2p{

	namespace Core{

		class Slot{
			public:
				inline bool operator<(const Slot& slot) const{
					return this < &slot;
				}

				inline bool operator==(const Slot& slot) const{
					return this == &slot;
				}

				inline bool operator!=(const Slot& slot) const{
					return this != &slot;
				}

		};

	}

}

#endif
