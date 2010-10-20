#ifndef OPENP2P_CORE_TIMEDSTREAM_HPP
#define OPENP2P_CORE_TIMEDSTREAM_HPP

namespace openp2p{

	namespace Core{

		class Buffer;

		class TimedStream: public Stream{
			public:
				TimedStream(Stream *, unsigned int);

				//TODO

		};

	}

}

#endif
