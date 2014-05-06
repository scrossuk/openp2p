#include <stdint.h>
#include <iostream>
#include <vector>

#include <p2p/Concurrency.hpp>
#include <p2p/TCP.hpp>
#include <p2p/Transport.hpp>

using namespace p2p;

template <typename T>
class Container {
	public:
		~Container() {
			for (typename std::vector<T*>::iterator i = data_.begin(); i != data_.end(); ++i) {
				delete(*i);
			}
		}
		
		T* add(T* t) {
			data_.push_back(t);
			return t;
		}
		
	private:
		std::vector<T*> data_;
		
};

class ClientThread: public Runnable {
	public:
		ClientThread() { }
		virtual ~ClientThread() { }
		
		TCP::Stream& getTCPStream() {
			return tcpStream_;
		}
		
		void run() {
			try {
				std::cout << "---Started transfer" << std::endl;
				
				BinaryIOStream stream(tcpStream_);
				
				for (unsigned int i = 0; i < 1000; i += 2) {
					const uint32_t v = Binary::ReadUint32(stream.input());
					
					if (v != i) {
						std::cout << "Wrong number: " << v << ", Expected: " << (i + 1) << " - Terminating connection" << std::endl;
						return;
					}
					
					std::cout << "Received: " << i << std::endl;
					
					usleep(10000);
					
					Binary::WriteUint32(stream.output(), i + 1);
					
					std::cout << "Sent: " << (i + 1) << std::endl;
				}
				
				std::cout << "---Successfully completed transfer" << std::endl;
			} catch (const std::exception& e) {
				std::cout << "Failed with error: " << e.what() << std::endl;
			}
		}
		
		void cancel() {
			// tcpStream_.close();
		}
		
	private:
		TCP::Stream tcpStream_;
		
};

int main() {
	TCP::Acceptor acceptor(45556);
	
	Container<ClientThread> clientThreads;
	Container<Thread> threads;
	
	while (true) {
		ClientThread* c = clientThreads.add(new ClientThread());
		
		std::cout << "Start accept." << std::endl;
		
		if (acceptor.accept(c->getTCPStream())) {
			//thread per connection
			threads.add(new Thread(*c));
		} else {
			std::cout << "Error: Accept failed." << std::endl;
			return 1;
		}
	}
	
	return 0;
}

