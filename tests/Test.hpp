#ifndef TEST_HPP
#define TEST_HPP

#include <string>

class Output {
	public:
		void warning(const std::string& str);
		
		void error(const std::string& str);
		
};

class Test {
	public:
		virtual std::string name() = 0;
		
		virtual bool run(Output& output) = 0;
		
};



#endif
