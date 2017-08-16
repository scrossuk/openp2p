#ifndef P2P_TCP_ACCEPTOR_HPP
#define P2P_TCP_ACCEPTOR_HPP

#include <memory>

#include <p2p/TCP/TCP_api.hpp>
#include <p2p/TCP/Stream.hpp>

namespace p2p {

	namespace TCP {
	
		class TCP_OPENP2P_API Acceptor {
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
