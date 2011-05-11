#ifndef OPENP2P_OFTORRENT_XORSTREAM_HPP
#define OPENP2P_OFTORRENT_XORSTREAM_HPP

#include <stdint.h>
#include <cstddef>
#include <boost/scoped_array.hpp>
#include <OpenP2P/CacheStream.hpp>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P{

	namespace OFTorrent{

		class XORIStream: public IStream{
			public:
				XORIStream(IStream& source1, IStream& source2);

				std::size_t readSome(uint8_t *, std::size_t);

				void cancel();

			private:
				CacheStream cacheStream_;
				IStream& source2_;

		};

	}

}

#endif
