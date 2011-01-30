#include <OpenP2P/CancelFunction.hpp>
#include <OpenP2P/Dispatcher.hpp>
#include <OpenP2P/NoBlock.hpp>
#include <OpenP2P/NullFunction.hpp>
#include <OpenP2P/WaitCallback.hpp>

namespace OpenP2P{

	CancelFunction NoBlock(WaitCallback callback){
		Schedule(callback);
		return NullFunction;
	}

}

