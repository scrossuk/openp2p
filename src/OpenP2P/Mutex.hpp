#ifndef OPENP2P_MUTEX_HPP
#define OPENP2P_MUTEX_HPP

namespace OpenP2P{

	class Mutex{
		public:
			inline operator boost::mutex&(){
				return internalMutex_;
			}

		private:
			boost::mutex internalMutex_;

	};

}

#endif
