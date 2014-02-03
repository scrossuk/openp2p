#ifndef FUSE_PATH_HPP
#define FUSE_PATH_HPP

#include <functional>
#include <string>
#include <vector>

namespace FUSE {
	
	class Path {
		public:
			Path();
			Path(const Path&) = default;
			Path(Path&& path) noexcept;
			Path(const std::string& pathString);
			
			Path& operator=(Path path);
			
			bool empty() const;
			
			size_t size() const;
			
			const std::string& front() const;
			
			const std::string& back() const;
			
			const std::string& at(size_t index) const;
			
			Path parent() const;
			
			bool hasChild(const Path& child) const;
			
			Path rebase(const Path& oldParent, const Path& newParent) const;
			
			Path subpath(size_t position, size_t length) const;
			
			std::size_t hash() const;
			
			std::string toString() const;
			
			Path operator+(const std::string& component) const;
			Path operator+(const Path& path) const;
			
			bool operator==(const Path&) const;
			bool operator!=(const Path&) const;
			bool operator<(const Path&) const;
			
		private:
			std::vector<std::string> components_;
		
	};
	
}

namespace std {
	
	template<>
	struct hash<FUSE::Path> {
		std::size_t operator()(const FUSE::Path& path) const {
			return path.hash();
		}
	};
	
}

#endif
