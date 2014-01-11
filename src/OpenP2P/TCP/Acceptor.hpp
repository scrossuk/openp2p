#ifndef OPENP2P_TCP_ACCEPTOR_HPP
#define OPENP2P_TCP_ACCEPTOR_HPP

#include <memory>

#include <OpenP2P/TCP/Stream.hpp>

namespace OpenP2P {

	namespace TCP {
	
		class Acceptor {
			public:
				Acceptor(uint16_t port);
				~Acceptor();
				
				bool accept(Stream& stream);
				
			private:
				// Non-copyable.
				Acceptor(const Acceptor&) = delete;
				Acceptor& operator=(Acceptor) = delete;
				
				std::unique_ptr<struct AcceptorImpl> impl_;
				
		};
		
	}
	
}

#endif
