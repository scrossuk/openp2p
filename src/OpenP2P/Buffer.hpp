#ifndef OPENP2P_BUFFER_HPP
#define OPENP2P_BUFFER_HPP

#include <stdint.h>
#include <cstddef>
#include <boost/shared_ptr.hpp>

namespace OpenP2P{

	class BufferTree{
		public:
			virtual boost::shared_ptr<BufferTree> left() = 0;

			virtual boost::shared_ptr<BufferTree> right() = 0;

			virtual const uint8_t * data() = 0;

			virtual std::size_t size() = 0;

	};

	class Buffer{
		public:
			Buffer();
			Buffer(const uint8_t *, std::size_t);
			Buffer(const Buffer&);
			Buffer(const Buffer&, const Buffer&);

			Buffer operator+(const Buffer&) const;

			boost::shared_ptr<BufferTree> tree() const;

			std::size_t size() const;

		private:
			boost::shared_ptr<BufferTree> tree_;

	};

	void printBuffer(const Buffer&);

}

#endif
