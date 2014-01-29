#include <string.h>

#include <string>

#include "Path.hpp"

namespace FUSE {
	
	Path ParsePath(const std::string& rawPath) {
		Path path;
		
		const char* position = rawPath.c_str();
		
		while (*position != '\0') {
			const char* dividerPosition = strchr(position, '/');
			
			if (dividerPosition == nullptr) {
				path.emplace_back(position);
				break;
			} else if (position < dividerPosition) {
				path.emplace_back(position, dividerPosition);
			}
			
			position = dividerPosition + 1;
		}
		
		return path;
	}
	
	std::string PathToString(const Path& path) {
		std::string s = "Path(";
		for (size_t i = 0; i < path.size(); i++) {
			if (i != 0) s += ", ";
			s += "'" + path.at(i) + "'";
		}
		s += ")";
		return s;
	}
	
}

