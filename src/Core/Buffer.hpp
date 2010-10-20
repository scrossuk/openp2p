#ifndef OPENP2P_CORE_BUFFER_HPP
#define OPENP2P_CORE_BUFFER_HPP

#include <string>
#include "Types.hpp"
#include "SharedPtr.hpp"
#include "SharedCount.hpp"

namespace openp2p{

	namespace Core{

		const unsigned int MaxBufferSize = 256;

		class BufferTree: public SharedCount{
			public:
				virtual SharedPtr<BufferTree> GetLeft() = 0;

				virtual SharedPtr<BufferTree> GetRight() = 0;

				virtual const Uint8 * GetData() const = 0;

				virtual unsigned int GetSize() const = 0;

		};

		class Buffer{
			public:
				Buffer();
				Buffer(Uint8);
				Buffer(Uint16);
				Buffer(Uint32);
				Buffer(const Uint8 *, unsigned int);
				Buffer(const std::string&);
				Buffer(const Buffer&);
				Buffer(const Buffer&, const Buffer&);

				Buffer operator+(const Buffer&) const;

				SharedPtr<BufferTree> GetTree() const;

				unsigned int Size() const;

			private:
				SharedPtr<BufferTree> mTree;

		};

		class BufferOutOfBoundsException{
			public:
				inline BufferOutOfBoundsException(){ }

		};

	}

}

#endif
