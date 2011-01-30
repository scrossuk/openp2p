#ifndef OPENP2P_RPCPROTOCOL_HPP
#define OPENP2P_RPCPROTOCOL_HPP

#include <iostream>
#include <stdint.h>
#include <string>
#include <boost/function.hpp>
#include <boost/ref.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Generator.hpp>
#include <OpenP2P/IdMap.hpp>
#include <OpenP2P/IStream.hpp>
#include <OpenP2P/OStream.hpp>
#include <OpenP2P/Signal.hpp>
#include <OpenP2P/Socket.hpp>
#include <OpenP2P/Wait.hpp>

namespace OpenP2P {

	template <typename EndpointType, typename IdType>
	class RPCProtocol : boost::noncopyable{
		typedef std::map < std::string, boost::function < void (const EndpointType&, IStream&, OStream&) > > MapType;
		typedef typename MapType::iterator IteratorType;

		public:
			RPCProtocol(Socket<EndpointType>& socket, Generator<IdType>& generator)
					: socket_(socket), generator_(generator), rpcs_(generator),
					internalThread_(boost::bind(&RPCProtocol::internalThread, this)) { }

			~RPCProtocol() {
				endSignal_.activate();
				internalThread_.join();
			}

			template <typename RequestType, typename ReplyType>
			class Handler{
				public:
					Handler(RPCProtocol<EndpointType, IdType>& protocol, const std::string& name, boost::function < ReplyType (const EndpointType&, const RequestType&) > handler)
						: protocol_(protocol), name_(name)
					{
						protocol_.rpcTypes_[name_] = boost::bind(&Handler<RequestType, ReplyType>::onRequest, this, handler, _1, _2, _3);
					}

					~Handler(){
						protocol_.rpcTypes_.erase(name_);
					}

					boost::optional<ReplyType> send(const EndpointType& endpoint, const RequestType& request){
						Signal signal;
						ReplyType reply;
						IdType id = protocol_.rpcs_.insert(boost::bind(&Handler<RequestType, ReplyType>::onReply, this, boost::ref(reply), boost::ref(signal), _1));

						BufferBuilder builder;
						builder << uint8_t(name_.length());
						builder << name_;
						builder << id;
						builder << request;
						protocol_.socket_.send(endpoint, *builder);
						Wait(signal);
						return boost::make_optional(reply);
					}

				private:
					void onRequest(boost::function < ReplyType (const EndpointType&, const RequestType&) > handler, const EndpointType& node, IStream& in, OStream& out) {
						RequestType request;
						if (!(in >> request)) {
							return;
						}
						out << (handler(node, request));
					}

					void onReply(ReplyType& reply, Signal& signal, IStream& in) {
						if (!(in >> reply)) {
							return;
						}
						signal.activate();
					}

					RPCProtocol<EndpointType, IdType>& protocol_;
					std::string name_;

			};

		private:
			template <typename RequestType, typename ReplyType>
			friend class Handler;

			void internalThread() {
				EndpointType endpoint;
				Buffer buffer;
				IdType rpcId;

				while (socket_.receive(endpoint, buffer, endSignal_)) {
					BufferIterator iterator(buffer);
					uint8_t nameLen;
					iterator >> nameLen;

					if (nameLen != 0) {
						//Receiving an rpc request
						std::string rpcName(nameLen, 0);

						if (iterator >> rpcName >> rpcId) {
							IteratorType p = rpcTypes_.find(rpcName);
							if (p != rpcTypes_.end()) {
								BufferBuilder builder;
								builder << uint8_t(0) << rpcId;
								(p->second)(endpoint, iterator, builder);
								socket_.send(endpoint, *builder);
							}
						}
					}
					else {
						//Receiving an rpc reply
						if (iterator >> rpcId) {
							boost::optional< boost::function<void (IStream&)> > p = rpcs_.find(rpcId);
							if (p) {
								(*p)(iterator);
							}
						}
					}
				}
			}

			Socket<EndpointType>& socket_;
			Generator<IdType>& generator_;
			IdMap < IdType, boost::function < void (IStream&) > > rpcs_;
			MapType rpcTypes_;
			Signal endSignal_;
			boost::thread internalThread_;

	};

}

#endif
