#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <optional>
#include <set>
#include <vulkan/vulkan.hpp>


//#include "oldVulk.h"
#include "logging.h"
#include "appWindow.h"
#include "vulkanRenderer.h"

void runNew() {
	// Setup window
	auto appWindow = AppWindow(800, 600);
	// Create renderer
	auto renderer = VulkanRenderer();
	renderer.initializeInstance(true);
	// Create surface which is displayed by the app window
	vk::SurfaceKHR surface = appWindow.createSurface(renderer.instance);
	// Initialize a gpu device that is compatable with the screen
	renderer.initializePhysicalAndLogicalDevice(surface);		
	int width, height;
	appWindow.getFramebufferSize(&width, &height);

	// Test function for creating external memory
	//renderer.createImage(width, height);

	renderer.createSwapchainAndMemory(surface, width, height);
	renderer.createRenderPass();
	renderer.createGraphicsPipeline(surface);

	//while (!appWindow.shouldClose()) {
		appWindow.update();
		renderer.render();
	//}
	renderer.finish();
	log("Success");
}

int main() {
	log("Started!");

	// Catch errors to print out
	try {
		runNew();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::cin.get();
		return EXIT_FAILURE;
	}

	// Don't close until user input
	std::cin.get();
	return EXIT_SUCCESS;
}