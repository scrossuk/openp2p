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

