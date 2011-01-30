#ifndef OPENP2P_IOSTREAM_HPP
#define OPENP2P_IOSTREAM_HPP

#include <OpenP2P/IStream.hpp>
#include <OpenP2P/OStream.hpp>

namespace OpenP2P{

	class IOStream: public IStream, public OStream{ };

}

#endif
