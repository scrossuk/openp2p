#ifndef OPENP2P_DESTROYHANDLER_HPP
#define OPENP2P_DESTROYHANDLER_HPP

#include <boost/shared_ptr.hpp>

namespace OpenP2P{

	class DestroyHandler{
		public:
			DestroyHandler(boost::function<void ()> f) : ptr_(new Internal(f)){ }
			DestroyHandler(const DestroyHandler& handler) : ptr_(handler.ptr_){ }

		private:
			struct Internal{
				boost::function<void ()> handler;

				Internal(boost::function<void ()> h) : handler(h){ }

				~Internal(){
					handler();
				}
			};

			boost::shared_ptr<Internal> ptr_;

	};

}

#endif
