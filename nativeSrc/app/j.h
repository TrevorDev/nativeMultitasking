#pragma once
#include <iostream>
#include <string>

void jlog(void* obj) {
	std::cout << obj << std::endl;
}
void jlog(int obj) {
	std::cout << obj << std::endl;
}
void jlog(const char* obj) {
	std::cout << obj << std::endl;
}

void jwaitForEnter(){
    std::cin.get();
}