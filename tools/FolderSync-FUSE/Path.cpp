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
	
}

