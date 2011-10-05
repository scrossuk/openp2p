#ifndef OPENP2P_FILESTREAM_HPP
#define OPENP2P_FILESTREAM_HPP

#include <cstdio>
#include <string>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P{

	class FileIStream: public InputStream{
		public:
			inline FileIStream() : handle_(0){ }

			inline FileIStream(const std::string& fileName){
				handle_ = fopen(fileName.c_str(), "rb");
			}

			inline ~FileIStream(){
				close();
			}

			inline bool open(const std::string& fileName){
				close();
				handle_ = fopen(fileName.c_str(), "rb");
				return isOpen();
			}

			inline bool isOpen(){
				return handle_ != 0;
			}
			
			inline std::size_t waitForData(Timeout){
				if(!isOpen()) return 0;
				const std::size_t currentPos = ftell(handle_);
				fseek(handle_, 0, SEEK_END);
				const std::size_t endPos = ftell(handle_);
				fseek(handle_, currentPos, SEEK_SET);
				return endPos - currentPos;
			}

			inline bool read(uint8_t * data, std::size_t size, Timeout){
				if(!isOpen()) return false;
				return fread(data, 1, size, handle_) == size;
			}

			inline void close(){
				if(handle_ != 0){
					fclose(handle_);
					handle_ = 0;
				}
			}

		private:
			FILE * handle_;

	};

	class FileOStream: public OutputStream{
		public:
			inline FileOStream() : handle_(0){ }

			inline FileOStream(const std::string& fileName){
				handle_ = fopen(fileName.c_str(), "wb");
			}

			inline ~FileOStream(){
				close();
			}

			inline bool open(const std::string& fileName){
				close();
				handle_ = fopen(fileName.c_str(), "wb");
				return isOpen();
			}

			inline bool isOpen(){
				return handle_ != 0;
			}
			
			inline std::size_t waitForSpace(Timeout){
				if(!isOpen()) return 0;
				return std::numeric_limits<std::size_t>::max() - ftell(handle_);
			}

			inline bool write(const uint8_t * data, std::size_t size, Timeout){
				if(!isOpen()) return false;
				return fwrite(data, 1, size, handle_) == size;
			}

			inline void close(){
				if(handle_ != 0){
					fclose(handle_);
					handle_ = 0;
				}
			}

		private:
			FILE * handle_;

	};

}

#endif
