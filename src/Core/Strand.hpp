#ifndef OPENP2P_CORE_STRAND_HPP
#define OPENP2P_CORE_STRAND_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/detail/wrapped_handler.hpp>

#include "Dispatcher.hpp"

namespace openp2p{

	namespace Core{

		class Strand{
			public:
				Strand() : mStrand(GetGlobalIOService()){ }

				template <typename Handler> boost::asio::detail::wrapped_handler<boost::asio::strand, Handler> Wrap(Handler handler){
					return mStrand.wrap(handler);
				}

				template <typename Handler> void Schedule(Handler handler){
					mStrand.post(handler);
				}

			private:
				boost::asio::io_service::strand mStrand;

		};

	}

}

#endif
