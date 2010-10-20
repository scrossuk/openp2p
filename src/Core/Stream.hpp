#ifndef OPENP2P_CORE_STREAM_HPP
#define OPENP2P_CORE_STREAM_HPP

#include <boost/function.hpp>

namespace openp2p{

	namespace Core{

		class Buffer;

		class Stream{
			public:
				virtual void Data(Buffer) = 0;

				virtual ~Stream(){ }

		};

		typedef boost::function<Stream * (Stream *)> StreamBuilder;

	}

}

#endif
