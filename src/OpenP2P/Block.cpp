#include <boost/bind.hpp>

#include <OpenP2P/Block.hpp>
#include <OpenP2P/CancelFunction.hpp>
#include <OpenP2P/Dispatcher.hpp>
#include <OpenP2P/WaitCallback.hpp>

namespace OpenP2P{

	CancelFunction Block(WaitCallback callback){
		return boost::bind(Schedule, callback);
	}

}

