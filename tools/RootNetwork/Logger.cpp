#include <stdio.h>

#include <mutex>
#include <string>

#include "Logger.hpp"

Logger::Logger() { }

Logger::~Logger() {
	printf("\r");
	fflush(stdout);
}

void Logger::showPrompt() {
	printf("\r$ ");
	fflush(stdout);
}

void Logger::log(const std::string& message) {
	std::lock_guard<std::mutex> lock(mutex_);
	printf("\r%s\n", message.c_str());
	printf("$ ");
	fflush(stdout);
}

std::string readLine() {
	std::string line;
	
	while (true) {
		const int c = getc(stdin);
		if (c == EOF) {
			return "";
		}
		
		if (c == '\n') {
			if (!line.empty()) {
				break;
			} else {
				continue;
			}
		}
		
		line += (char) c;
	}
	
	return line;
}

