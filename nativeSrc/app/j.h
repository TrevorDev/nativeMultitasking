#pragma once
#include <iostream>
#include <string>
#include <fstream>

void jlog(void* obj) {
	std::cout << obj << std::endl;
}
void jlog(std::string obj) {
	std::cout << obj << std::endl;
}
void jlog(int obj) {
	std::cout << obj << std::endl;
}
void jlog(float obj) {
	std::cout << obj << std::endl;
}
void jlog(unsigned long long obj) {
	std::cout << obj << std::endl;
}
void jlog(const char* obj) {
	std::cout << obj << std::endl;
}

void jwaitForEnter(){
    std::cin.get();
}

static std::vector<char> jreadFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t) file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}