#ifndef OPENP2P_CORE_FIXEDSTREAM_HPP
#define OPENP2P_CORE_FIXEDSTREAM_HPP

#include "Stream.hpp"
#include "Buffer.hpp"
#include "BufferQueue.hpp"

namespace openp2p{

	namespace Core{

		class Buffer;

		class FixedStream: public Stream{
			public:
				FixedStream(Stream *, unsigned int);

				~FixedStream();

				void Data(Buffer&);

			private:
				Stream * mStream;

				unsigned int mSize;

				BufferQueue mData;

		};

	}

}

#endif
