#include <iostream>
#include <sstream>
#include <OpenP2P.hpp>
#include <OpenP2P/Stream/BinaryStream.hpp>
#include <OpenP2P/OFTorrent.hpp>

using namespace OpenP2P;

class BlockGen: public OFTorrent::OutputStreamGenerator {
	public:
		BlockGen() : count_(0) { }
		
		OStream& getNextOutputStream() {
			std::ostringstream s;
			s << "out" << count_ << ".txt";
			count_++;
			fileStream.open(s.str());
			return fileStream;
		}
		
	private:
		size_t count_;
		FileOStream fileStream;
		
};

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cout << "create [filename]" << std::endl;
		return 0;
	}
	
	FileIStream fileStream(argv[1]);
	
	if (!fileStream.isOpen()) {
		std::cout << "Failed to open file" << std::endl;
	}
	
	NullIStream nullStream;
	
	OFTorrent::XORStream xorStream(fileStream, nullStream);
	
	BlockGen blockGen;
	
	OFTorrent::BlockStream blockStream(blockGen, OFTorrent::BLOCKSIZE_512KB);
	
	Binary::MoveData(xorStream, blockStream);
	
	return 0;
}

