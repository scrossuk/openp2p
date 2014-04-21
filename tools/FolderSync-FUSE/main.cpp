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

#include <p2p/FolderSync/Block.hpp>
#include <p2p/FolderSync/BlockId.hpp>
#include <p2p/FolderSync/Database.hpp>
#include <p2p/FolderSync/Directory.hpp>
#include <p2p/FolderSync/FileDatabase.hpp>
#include <p2p/FolderSync/MemDatabase.hpp>
#include <p2p/FolderSync/Node.hpp>

#include "NodeSystem.hpp"
#include "FileSystemWrapper.hpp"

using namespace p2p;

int main(int argc, char** argv) {
	if (argc != 3) {
		printf("Usage: %s [mount point] [block-path]\n", argv[0]);
		return 1;
	}
	
	printf("Max file size: %.2f GiB.\n",
		double(p2p::FolderSync::NODE_MAX_BYTES) / double(1024 * 1024 * 1024));
	
	const std::string mountPoint = argv[1];
	const std::string blockPath = argv[2];
	
	//FolderSync::MemDatabase database;
	FolderSync::FileDatabase database(blockPath);
	FolderSync::NodeSystem nodeSystem(database);
	FolderSync::FileSystemWrapper fileSystem(nodeSystem);
	
	return FUSE::run(mountPoint, fileSystem);
}

