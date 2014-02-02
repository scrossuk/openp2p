#ifndef FUSE_FILESYSTEM_HPP
#define FUSE_FILESYSTEM_HPP

#include <memory>
#include <string>

#include <sys/types.h>

#include <FUSE/Handle.hpp>
#include <FUSE/Path.hpp>

namespace FUSE {

	class FileSystem {
		public:
			inline virtual ~FileSystem() { }
			
			/**
			 * \brief Open a file.
			 */
			virtual Handle openFile(const Path& path) = 0;
			
			/**
			 * \brief Close a file.
			 */
			virtual void closeFile(Handle handle) = 0;
			
			/**
			 * \brief Read file.
			 */
			virtual size_t readFile(Handle handle, size_t offset, uint8_t* buffer, size_t size) const = 0;
			
			/**
			 * \brief Write file.
			 */
			virtual size_t writeFile(Handle handle, size_t offset, const uint8_t* buffer, size_t size) = 0;
			
			/**
			 * \brief Resize file.
			 */
			virtual void resize(const Path& path, size_t size) = 0;
			
			/**
			 * \brief Open a directory.
			 */
			virtual Handle openDirectory(const Path& path) = 0;
			
			/**
			 * \brief Close a directory.
			 */
			virtual void closeDirectory(Handle handle) = 0;
			
			/**
			 * \brief Read child node names.
			 */
			virtual std::vector<std::string> readDirectory(Handle handle) const = 0;
			
			/**
			 * \brief Create AND open a file.
			 */
			virtual Handle createAndOpenFile(const Path& path, mode_t mode) = 0;
			
			/**
			 * \brief Remove file.
			 */
			virtual void removeFile(const Path& path) = 0;
			
			/**
			 * \brief Create directory.
			 */
			virtual void createDirectory(const Path& path, mode_t mode) = 0;
			
			/**
			 * \brief Remove directory.
			 */
			virtual void removeDirectory(const Path& path) = 0;
			
			/**
			 * \brief Rename a node.
			 *
			 * This should atomically replace the node at 'destPath'
			 * with the node at 'sourcePath', deleting any node that
			 * currently exists at 'destPath'.
			 */
			virtual void rename(const Path& sourcePath, const Path& destPath) = 0;
			
			/**
			 * \brief Get node attributes.
			 */
			virtual struct stat getAttributes(const Path& path) const = 0;
			
			/**
			 * \brief Change 'mode' (permissions) for a node.
			 */
			virtual void changeMode(const Path& path, mode_t mode) = 0;
			
			/**
			 * \brief Change owner for a node.
			 */
			virtual void changeOwner(const Path& path, uid_t user, gid_t group) = 0;
			
	};
	
	int run(const std::string& mountPoint, FileSystem& fileSystem);
	
}

#endif
