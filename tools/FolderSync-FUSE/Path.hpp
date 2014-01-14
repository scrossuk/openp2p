#ifndef OPENP2P_FOLDERSYNC_FUSE_PATH_HPP
#define OPENP2P_FOLDERSYNC_FUSE_PATH_HPP

#include <string>
#include <vector>

namespace FUSE {
	
	typedef std::vector<std::string> Path;
	
	Path ParsePath(const std::string& rawPath);
	
}

#endif
