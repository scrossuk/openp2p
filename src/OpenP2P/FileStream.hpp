#ifndef OPENP2P_FILESTREAM_HPP
#define OPENP2P_FILESTREAM_HPP

#include <cstdio>
#include <string>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P {

	class FileIStream: public IStream {
		public:
			inline FileIStream() : handle_(NULL) { }
			
			inline FileIStream(const std::string& fileName) {
				handle_ = fopen(fileName.c_str(), "rb");
			}
			
			inline ~FileIStream() {
				close();
			}
			
			inline bool open(const std::string& fileName) {
				close();
				handle_ = fopen(fileName.c_str(), "rb");
				return isOpen();
			}
			
			inline bool isOpen() const {
				return handle_ != NULL;
			}
			
			inline bool isValid() const {
				if (!isOpen()) return false;
				
				const size_t currentPos = ftell(handle_);
				fseek(handle_, 0, SEEK_END);
				const size_t endPos = ftell(handle_);
				fseek(handle_, currentPos, SEEK_SET);
				
				return endPos > currentPos;
			}
			
			inline Event::Source eventSource() const {
				return Event::Source();
			}
			
			inline size_t read(uint8_t* data, size_t size) {
				if (!isOpen()) {
					return 0;
				}
				
				return fread(data, 1, size, handle_);
			}
			
			inline void close() {
				if (!isOpen()) return;
				
				fclose(handle_);
				handle_ = NULL;
			}
			
		private:
			FILE* handle_;
			
	};
	
	class FileOStream: public OStream {
		public:
			inline FileOStream() : handle_(NULL) { }
			
			inline FileOStream(const std::string& fileName) {
				handle_ = fopen(fileName.c_str(), "wb");
			}
			
			inline ~FileOStream() {
				close();
			}
			
			inline bool open(const std::string& fileName) {
				close();
				handle_ = fopen(fileName.c_str(), "wb");
				return isOpen();
			}
			
			inline bool isOpen() const {
				return handle_ != NULL;
			}
			
			inline bool isValid() const {
				return isOpen();
			}
			
			inline Event::Source eventSource() const {
				return Event::Source();
			}
			
			inline size_t write(const uint8_t* data, size_t size) {
				if (!isOpen()) {
					return false;
				}
				
				return fwrite(data, 1, size, handle_);
			}
			
			inline void close() {
				if (!isOpen()) return;
				
				fclose(handle_);
				handle_ = NULL;
			}
			
		private:
			FILE* handle_;
			
	};
	
}

#endif
