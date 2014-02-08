#ifndef OPENP2P_STRING_HPP
#define OPENP2P_STRING_HPP

#include <string>

std::string STR(const char * format, ...)
	__attribute__((format(printf, 1, 2)));

#endif
