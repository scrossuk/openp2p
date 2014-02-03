#include <string.h>

#include <stdexcept>
#include <string>
#include <vector>

#include <boost/functional/hash.hpp>

#include <FUSE/Path.hpp>

namespace FUSE {
	
	Path::Path() { }
	
	Path::Path(Path&& path) noexcept
		: Path() {
		std::swap(components_, path.components_);
	}
	
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
	
	Path& Path::operator=(Path path) {
		std::swap(components_, path.components_);
		return *this;
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
	
	bool Path::hasChild(const Path& child) const {
		if (child.size() < size()) return false;
		
		for (size_t i = 0; i < size(); i++) {
			if (child.at(i) != at(i)) return false;
		}
		
		return true;
	}
	
	Path Path::rebase(const Path& oldParent, const Path& newParent) const {
		assert(size() >= oldParent.size());
		assert(*this == oldParent || oldParent.hasChild(*this));
		
		return newParent + subpath(oldParent.size(), size() - oldParent.size());
	}
	
	Path Path::subpath(size_t position, size_t length) const {
		Path subPath;
		for (size_t i = position; i < length; i++) {
			subPath.components_.push_back(at(i));
		}
		return subPath;
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
	
	Path Path::operator+(const std::string& component) const {
		Path path;
		path.components_ = components_;
		path.components_.push_back(component);
		return path;
	}
	
	Path Path::operator+(const Path& path) const {
		Path newPath;
		newPath.components_ = components_;
		for (size_t i = 0; i < path.size(); i++) {
			newPath.components_.push_back(path.at(i));
		}
		return newPath;
	}
	
	bool Path::operator==(const Path& other) const {
		return components_ == other.components_;
	}
	
	bool Path::operator!=(const Path& other) const {
		return components_ != other.components_;
	}
	
	bool Path::operator<(const Path& other) const {
		return components_ < other.components_;
	}
	
}

