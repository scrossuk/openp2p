#ifndef P2P_UTIL_STRING_HPP
#define P2P_UTIL_STRING_HPP

#include <string>

std::string STR(const char * format, ...)
	__attribute__((format(printf, 1, 2)));

#endif
