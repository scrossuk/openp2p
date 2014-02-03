#ifndef OPENP2P_FOLDERSYNC_HANDLEREF_HPP
#define OPENP2P_FOLDERSYNC_HANDLEREF_HPP

#include <FUSE/Handle.hpp>
#include <FUSE/Path.hpp>

namespace OpenP2P {

	namespace FolderSync {
	
		class NodeSystem;
		
		class HandleRef {
			public:
				HandleRef() noexcept;
				HandleRef(HandleRef&& ref) noexcept;
				HandleRef(NodeSystem& nodeSystem, FUSE::Handle handle);
				~HandleRef();
				
				HandleRef& operator=(HandleRef ref);
				
				FUSE::Handle get() const;
				
				FUSE::Handle release();
				
			private:
				// Non-copyable.
				HandleRef(const HandleRef&) = delete;
				
				NodeSystem* nodeSystem_;
				FUSE::Handle handle_;
				
		};
		
		HandleRef openPath(NodeSystem& nodeSystem, const FUSE::Path& path);
		
	}
	
}

#endif
