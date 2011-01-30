#ifndef OPENP2P_WAIT_HPP
#define OPENP2P_WAIT_HPP

#include <vector>
#include <OpenP2P/WaitHandler.hpp>

namespace OpenP2P{

	void Wait(WaitHandler);
	std::size_t Wait(WaitHandler, WaitHandler);
	std::size_t Wait(WaitHandler, WaitHandler, WaitHandler);
	std::size_t Wait(WaitHandler, WaitHandler, WaitHandler, WaitHandler);

}

#endif
