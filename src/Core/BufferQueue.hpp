#ifndef OPENP2P_CORE_BUFFERQUEUE_HPP
#define OPENP2P_CORE_BUFFERQUEUE_HPP

#include <queue>
#include "Types.hpp"
#include "Buffer.hpp"
#include "BufferIterator.hpp"
#include "DataSource.hpp"

namespace openp2p{

	namespace Core{

		class BufferQueue: public DataSource{
			public:
				BufferQueue();

				void Add(Buffer);

				unsigned int Size();

				void Read(Uint8 *, unsigned int);

				Buffer Read(unsigned int);

				Buffer ReadAll();

			private:
				unsigned int mSize, mCurrentRemaining;

				BufferIterator mIterator;

				std::queue<Buffer> mQueue;

		};

	}

}

#endif
