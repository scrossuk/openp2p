#ifndef OPENP2P_FILESTREAM_HPP
#define OPENP2P_FILESTREAM_HPP

#include <cstdio>
#include <string>
#include <OpenP2P/Stream.hpp>

namespace OpenP2P{

	class FileIStream: public IStream{
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

			inline std::size_t readSome(uint8_t * data, std::size_t requestedSize){
				if(!isOpen()) return 0;
				return fread((void *) data, 1, requestedSize, handle_);
			}

			inline void cancel(){ }

			inline void close(){
				if(handle_ != 0){
					fclose(handle_);
					handle_ = 0;
				}
			}

		private:
			FILE * handle_;

	};

	class FileOStream: public OStream{
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

			inline std::size_t writeSome(const uint8_t * data, std::size_t requestedSize){
				if(!isOpen()) return 0;
				return fwrite((const void *) data, 1, requestedSize, handle_);
			}

			inline void cancel(){ }

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
