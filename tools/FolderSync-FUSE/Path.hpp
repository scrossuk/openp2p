#ifndef OPENP2P_FOLDERSYNC_FUSE_PATH_HPP
#define OPENP2P_FOLDERSYNC_FUSE_PATH_HPP

#include <functional>
#include <string>
#include <vector>

namespace FUSE {
	
	class Path {
		public:
			Path();
			Path(const std::string& pathString);
			
			bool empty() const;
			
			size_t size() const;
			
			const std::string& front() const;
			
			const std::string& back() const;
			
			const std::string& at(size_t index) const;
			
			Path parent() const;
			
			std::size_t hash() const;
			
			std::string toString() const;
			
			bool operator==(const Path&) const;
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
