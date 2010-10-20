#ifndef OPENP2P_CORE_BUFFERITERATOR_HPP
#define OPENP2P_CORE_BUFFERITERATOR_HPP

#include <stack>
#include "SharedPtr.hpp"
#include "Types.hpp"
#include "Buffer.hpp"
#include "DataSource.hpp"

namespace openp2p{

	namespace Core{

		class BufferIterator: public DataSource{
			public:
				BufferIterator();

				BufferIterator(Buffer, unsigned int = 0);

				~BufferIterator();

				unsigned int GetPosition();
				
				unsigned int Remaining();

				void SetBuffer(Buffer, unsigned int = 0);

				void Seek(unsigned int);

				Buffer GetBuffer() const;

				void Read(Uint8 *, unsigned int);

				Buffer Read(unsigned int);
				
				Buffer Read();

			private:
				void MoveToSuccessor();

				Buffer * mBuffer;

				unsigned int mNodePos;

				unsigned int mPosition;

				std::stack< SharedPtr<BufferTree> > mStack;

		};

	}

}

#endif
