#include <iostream>
#include <string>

#include "vulkan.cpp"

// void main(std::string str){
//     std::cout << "Command: " << str << std::endl;
// }

int main(std::string str) {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}