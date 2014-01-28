#ifndef OPENP2P_FOLDERSYNC_FUSE_FILESYSTEM_HPP
#define OPENP2P_FOLDERSYNC_FUSE_FILESYSTEM_HPP

#include <stdint.h>

#include <memory>
#include <string>
#include <vector>

#include "Path.hpp"

namespace FUSE {

	class ErrorException {
		public:
			inline ErrorException(int e) :
				error_(e) { }
			
			inline int error() const {
				return error_;
			}
			
		private:
			int error_;
		
	};
	
	class OpenedDirectory {
		public:
			inline virtual ~OpenedDirectory() { }
			
			virtual std::vector<std::string> read() const = 0;
			
	};
	
	class OpenedFile {
		public:
			inline virtual ~OpenedFile() { }
			
			virtual size_t read(size_t offset, uint8_t* buffer, size_t size) const = 0;
			
			virtual size_t write(size_t offset, const uint8_t* buffer, size_t size) = 0;
			
	};
	
	class FileSystem {
		public:
			inline virtual ~FileSystem() { }
			
			/**
			 * \brief Create a file.
			 */
			virtual void createFile(const Path& path, mode_t mode) = 0;
			
			/**
			 * \brief Open a file.
			 */
			virtual std::unique_ptr<OpenedFile> openFile(const Path& path) = 0;
			
			/**
			 * \brief Unlink a file.
			 *
			 * This removes a hard link to a file.
			 */
			virtual void unlink(const Path& path) = 0;
			
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
			 * \brief Resize file.
			 */
			virtual void resize(const Path& path, size_t size) = 0;
			
			/**
			 * \brief Change 'mode' (permissions) for a node.
			 */
			virtual void changeMode(const Path& path, mode_t mode) = 0;
			
			/**
			 * \brief Change owner for a node.
			 */
			virtual void changeOwner(const Path& path, uid_t user, gid_t group) = 0;
			
			/**
			 * \brief Create a directory.
			 */
			virtual void createDirectory(const Path& path, mode_t mode) = 0;
			
			/**
			 * \brief Remove a directory.
			 */
			virtual void removeDirectory(const Path& path) = 0;
			
			/**
			 * \brief Open a directory.
			 */
			virtual std::unique_ptr<OpenedDirectory> openDirectory(const Path& path) = 0;
			
	};
	
	int run(const std::string& mountPoint, FileSystem& fileSystem);
	
}

#endif
