#include <vector>
#include "Test.hpp"
#include "BufferTest.hpp"

void Output::warning(const std::string& str) {
	std::cout << "WARNING: " << str << std::endl;
}

void Output::error(const std::string& str) {
	std::cout << "ERROR: " << str << std::endl;
}

int main() {
	BufferTest bufferTest;
	
	std::vector<Test*> tests_;
	tests_.push_back(&bufferTest);
	
	size_t failures = 0;
	Output output;
	
	std::vector<Test*>::iterator i;
	
	for (i = tests_.begin(); i != tests_.end(); ++i) {
		std::cout << "---Running test \"" << (*i)->name() << "\"" << std::endl;
		bool success = (*i)->run(output);
		std::cout << "---Test " << (success ? "Passed" : "Failed") << std::endl;
		
		if (!success) {
			failures++;
		}
	}
	
	std::cout << std::endl << "Finished with " << failures << " failure" << (failures == 1 ? "" : "s") << "." << std::endl;
	return 0;
}

