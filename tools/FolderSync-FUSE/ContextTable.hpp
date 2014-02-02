#ifndef OPENP2P_FOLDERSYNC_FUSE_CONTEXTTABLE_HPP
#define OPENP2P_FOLDERSYNC_FUSE_CONTEXTTABLE_HPP

#include <functional>
#include <string>
#include <vector>

namespace FUSE {
	
	template <typename T>
	struct ContextInfo {
		size_t refCount;
		unique_ptr<T> context;
		
		inline ContextInfo(unique_ptr<T>&& pContext)
			: refCount(0), context(std::move(pContext)) {
				assert(context.get() != nullptr);
			}
	};
	
	template <typename T>
	class ContextRef {
		public:
			ContextRef(ContextInfo<T>* info)
				: info_(info) {
				if (info_ != nullptr) info_->refCount++;
			}
			
			ContextRef(Context&& contextRef) noexcept
				: info_(contextRef.info_) {
				contextRef.info_ = nullptr;
			}
			
			~ContextRef() {
				if (info_ == nullptr) return;
				info_->refCount--;
				if (info->refCount == 0) {
					// TODO
				}
			}
			
			T* get() const {
				return info_ != nullptr ? info_->context.get() : nullptr;
			}
			
			T* operator->() const {
				assert(info_ != nullptr);
				return info_->context.get();
			}
			
			T& operator*() const {
				assert(info_ != nullptr);
				return *(info_->context);
			}
			
		private:
			// Non-copyable.
			ContextRef(const ContextRef<T>&) = delete;
			ContextRef<T>& operator=(ContextRef<T>) = delete;
			
			ContextInfo<T>* info_;
		
	};
	
	template <typename Key, typename T>
	class ContextTable {
		public:
			ContextTable() { }
			
			Context<T> insert(Key key, unique_ptr<T>&& context) {
				auto result = map_.emplace(std::move(key), ContextInfo<T>(std::move(context)));
				if (!result.second) {
					throw std::runtime_error("Context entry already exists.");
				}
				auto& iterator = result.first;
				return ContextRef(&(iterator->second));
			}
			
			ContextRef<T> find(const Key& key) {
				const auto iterator = map_.find(key);
				if (iterator == map_.end()) {
					return ContextRef(nullptr);
				}
				
				return ContextRef(&(iterator->second));
			}
			
		private:
			// Non-copyable.
			ContextTable(const ContextTable<Key, T>&) = delete;
			ContextTable<Key, T>& operator=(ContextTable<Key, T>) = delete;
			
			std::unordered_map<Key, ContextInfo> map_;
			
	};
	
}

#endif
