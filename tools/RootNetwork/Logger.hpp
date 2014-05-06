#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

#include <mutex>
#include <string>

class Logger {
	public:
		Logger();
		~Logger();
		
		void showPrompt();
		
		void log(const std::string& message);
		
	private:
		std::mutex mutex_;
		
};

std::string readLine();

#endif
