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
#include <OpenP2P/FolderSync/MemDatabase.hpp>
#include <OpenP2P/FolderSync/Node.hpp>

#include "NodeSystem.hpp"
#include "FileSystemWrapper.hpp"

using namespace OpenP2P;

int main(int argc, char** argv) {
	if (argc != 3) {
		printf("Usage: %s [mount point] [block-path]\n", argv[0]);
		return 1;
	}
	
	printf("Max file size: %.2f GiB.\n",
		double(OpenP2P::FolderSync::NODE_MAX_BYTES) / double(1024 * 1024 * 1024));
	
	const std::string mountPoint = argv[1];
	const std::string blockPath = argv[2];
	
	//FolderSync::MemDatabase database;
	FolderSync::FileDatabase database(blockPath);
	FolderSync::NodeSystem nodeSystem(database);
	FolderSync::FileSystemWrapper fileSystem(nodeSystem);
	
	return FUSE::run(mountPoint, fileSystem);
}

