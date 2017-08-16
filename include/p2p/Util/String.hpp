#ifndef P2P_UTIL_STRING_HPP
#define P2P_UTIL_STRING_HPP

#include <string>

#include <p2p/Util/Util_api.hpp>

std::string UTIL_OPENP2P_API STR(const char * format, ...)
	__attribute__((format(printf, 1, 2)));

#endif
