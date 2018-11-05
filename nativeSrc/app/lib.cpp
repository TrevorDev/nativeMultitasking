#include <iostream>
#include <string>
#include "vulkan.cpp"

HelloTriangleApplication app;
int initVulkan(std::string str) {
    try {
        app.initWindow();
        app.initVulkan();
        
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int render(std::string str) {
    try {
        app.mainLoop();
        app.cleanup();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}