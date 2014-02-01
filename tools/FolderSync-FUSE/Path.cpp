#include <string.h>

#include <stdexcept>
#include <string>
#include <vector>

#include <boost/functional/hash.hpp>

#include "Path.hpp"

namespace FUSE {
	
	Path::Path() { }
	
	Path::Path(const std::string& pathString) {
		const char* position = pathString.c_str();
		
		while (*position != '\0') {
			const char* dividerPosition = strchr(position, '/');
			
			if (dividerPosition == nullptr) {
				components_.emplace_back(position);
				return;
			} else if (position < dividerPosition) {
				components_.emplace_back(position, dividerPosition);
			}
			
			position = dividerPosition + 1;
		}
	}
	
	bool Path::empty() const {
		return components_.empty();
	}
	
	size_t Path::size() const {
		return components_.size();
	}
	
	const std::string& Path::front() const {
		return components_.front();
	}
	
	const std::string& Path::back() const {
		return components_.back();
	}
	
	const std::string& Path::at(size_t index) const {
		return components_.at(index);
	}
	
	Path Path::parent() const {
		if (empty()) {
			throw std::runtime_error("Can't get parent of empty path.");
		}
		
		auto endIterator = components_.end();
		--endIterator;
		
		Path parentPath;
		parentPath.components_.assign(components_.begin(), endIterator);
		return parentPath;
	}
	
	std::size_t Path::hash() const {
		return boost::hash_range(components_.begin(), components_.end());
	}
	
	std::string Path::toString() const {
		std::string s = "Path(";
		for (size_t i = 0; i < size(); i++) {
			if (i != 0) s += ", ";
			s += "'" + at(i) + "'";
		}
		s += ")";
		return s;
	}
	
	bool Path::operator==(const Path& other) const {
		return components_ == other.components_;
	}
	
	bool Path::operator<(const Path& other) const {
		return components_ < other.components_;
	}
	
}

