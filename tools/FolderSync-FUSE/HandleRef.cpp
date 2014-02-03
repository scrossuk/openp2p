#include <assert.h>

#include <utility>

#include <FUSE/Handle.hpp>
#include <FUSE/Path.hpp>

#include "HandleRef.hpp"
#include "NodeSystem.hpp"

namespace OpenP2P {

	namespace FolderSync {
	
		HandleRef::HandleRef() noexcept
			: nodeSystem_(nullptr), handle_(0) { }
			
		HandleRef::HandleRef(NodeSystem& nodeSystem, FUSE::Handle handle)
			: nodeSystem_(&nodeSystem), handle_(handle) {
			assert(nodeSystem_ != nullptr);
		}
		
		HandleRef::HandleRef(HandleRef&& ref) noexcept
			: HandleRef() {
			std::swap(nodeSystem_, ref.nodeSystem_);
			std::swap(handle_, ref.handle_);
		}
		
		HandleRef::~HandleRef() {
			if (nodeSystem_ == nullptr) {
				return;
			}
			
			nodeSystem_->closeNode(handle_);
		}
		
		HandleRef& HandleRef::operator=(HandleRef ref) {
			std::swap(nodeSystem_, ref.nodeSystem_);
			std::swap(handle_, ref.handle_);
			return *this;
		}
		
		FUSE::Handle HandleRef::get() const {
			assert(nodeSystem_ != nullptr);
			return handle_;
		}
		
		FUSE::Handle HandleRef::release() {
			assert(nodeSystem_ != nullptr);
			nodeSystem_ = nullptr;
			return handle_;
		}
		
		HandleRef openPath(NodeSystem& nodeSystem, const FUSE::Path& path) {
			auto handle = HandleRef(nodeSystem, nodeSystem.openRoot());
			
			for (size_t i = 0; i < path.size(); i++) {
				handle = HandleRef(nodeSystem, nodeSystem.openChild(handle.get(), path.at(i)));
			}
			
			return handle;
		}
		
	}
	
}

