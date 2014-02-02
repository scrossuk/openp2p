#include <assert.h>
#include <climits>
#include <cstdarg>
#include <cstring>
#include <ctime>

#include <fstream>
#include <set>
#include <string>
#include <unordered_map>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockId.hpp>
#include <OpenP2P/FolderSync/Database.hpp>
#include <OpenP2P/FolderSync/Directory.hpp>
#include <OpenP2P/FolderSync/FileDatabase.hpp>
#include <OpenP2P/FolderSync/Node.hpp>

#include "NodeSystem.hpp"
#include "FileSystemWrapper.hpp"

using namespace OpenP2P;

/*FolderSync::BlockId calculateNewId(FolderSync::Database& database, const FolderSync::BlockId& oldId, const FolderSync::BlockId& endNodeId, const FUSE::Path& path, size_t position = 0) {
	if (position == path.size()) {
		return endNodeId;
	}
	
	const std::string& pathComponent = path.at(position);
	
	FolderSync::Node node(database, oldId);
	
	if (node.type() != FolderSync::TYPE_DIRECTORY) {
		logFile() << "Parent is not a directory!" << std::endl;
		throw FUSE::ErrorException(ENOTDIR);
	}
	
	FolderSync::Directory directory(node);
	
	if (!directory.hasChild(pathComponent)) {
		logFile() << "Expected child '" << pathComponent << "' not found!" << std::endl;
		throw FUSE::ErrorException(ENOENT);
	}
	
	const auto childOldId = directory.getChild(pathComponent);
	const auto childNewId = calculateNewId(database, childOldId, endNodeId, path, position + 1);
	
	directory.updateChild(pathComponent, childNewId);
	
	node.flush();
	
	return node.blockId();
}*/

int main(int argc, char** argv) {
	if (argc != 3) {
		printf("Usage: %s [mount point] [block-path]\n", argv[0]);
		return 1;
	}
	
	printf("Max file size: %.2f GiB.\n",
		double(OpenP2P::FolderSync::NODE_MAX_BYTES) / double(1024 * 1024 * 1024));
	
	const std::string mountPoint = argv[1];
	const std::string blockPath = argv[2];
	
	FolderSync::FileDatabase database(blockPath);
	FolderSync::NodeSystem nodeSystem(database);
	FolderSync::FileSystemWrapper fileSystem(nodeSystem);
	
	return FUSE::run(mountPoint, fileSystem);
}

