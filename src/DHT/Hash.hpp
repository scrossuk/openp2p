#ifndef OPENP2P_DHT_HASH_HPP
#define OPENP2P_DHT_HASH_HPP

#include "../Core.hpp"

namespace openp2p{

	namespace DHT{

		class Hash{
			public:
				static const unsigned int SizeInBits = 256;
				static const unsigned int SizeInBytes = SizeInBits / 8;
				static Hash Unserialize(Core::BufferIterator&);

				Hash();
				Hash(Core::Uint8);
				Hash(const Core::Uint8 *);
				Hash(Core::Buffer);
				Hash(const Hash&);

				Hash operator^(const Hash&) const;

				bool operator==(const Hash&) const;
				bool operator!=(const Hash&) const;
				bool operator<(const Hash&) const;

				const Core::Uint8 * Data() const;

				Core::Buffer Serialize() const;

			private:
				Core::Uint8 mData[SizeInBytes];

		};

	}

}

#endif
