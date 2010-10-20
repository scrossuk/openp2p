#ifndef OPENP2P_CORE_SHAREDREADER_HPP
#define OPENP2P_CORE_SHAREDREADER_HPP

#include "Types.hpp"
#include <map>

namespace openp2p{

	namespace Core{

		class SharedStream;
		class Buffer;

		class SharedReader: public Stream{
			public:
				SharedReader(SharedStream&);

				~SharedReader();

				void Data(Buffer);

			private:
				SharedStream& mSharedStream;

				std::map< unsigned int, Ptr<Stream> > mStreamMap;

		};

	}

}

#endif
